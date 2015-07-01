#include <iostream>
#include <postgres_asio/postgres_asio.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>


int main(int argc, char *argv[])
{
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
    std::string connect_string = "user=postgres password=postgres dbname=test";
    
    if (argc > 1)
    {
        connect_string += std::string("host=") + argv[1];
    }

    boost::asio::io_service fg_ios;
    boost::asio::io_service bg_ios;
    std::auto_ptr<boost::asio::io_service::work> work2(new boost::asio::io_service::work(fg_ios));
    std::auto_ptr<boost::asio::io_service::work> work1(new boost::asio::io_service::work(bg_ios));
    boost::thread fg(boost::bind(&boost::asio::io_service::run, &fg_ios));
    boost::thread bg(boost::bind(&boost::asio::io_service::run, &bg_ios));


    // precondition CREATE TABLE postgres_asio_sample  ( id integer, val text )


    for (int i = 0; i != 500; ++i)
    {
        auto connection = boost::make_shared<postgres_asio::connection>(fg_ios, bg_ios);
        connection->set_log_id("xxxx-xxxx-xx" + std::to_string(i));
        connection->connect(connect_string, [connection](int ec)
        {
            if (!ec)
            {
                std::string statement = "insert into postgres_asio_sample (id, val) VALUES\n";
                size_t items = 10000;
                for (int i = 0; i != items; ++i)
                {
                    const char* ch = i < (items - 1) ? ",\n" : ";\n";
                    std::string val = to_string(boost::uuids::random_generator()());
                    statement += " (" + std::to_string(i) + ", '" + val + "')" + ch;
                }

                std::cerr << connection->get_log_id() << " inserting!!" << std::endl;
                connection->exec(statement, [connection](int ec, boost::shared_ptr<PGresult> res)
                {
                    if (ec)
                    {
                        std::cerr << connection->get_log_id() << " insert failed ec:" << ec << " last_error:" << connection->last_error() << std::endl;
                        return;
                    }
                    std::cerr << connection->get_log_id() << " done!!" << std::endl;
                });
            }
        });
    }

    while (true)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }

    work1.reset();
    work2.reset();
    //bg_ios.stop();
    //fg_ios.stop();
    bg.join();
    fg.join();
}