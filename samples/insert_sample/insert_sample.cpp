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
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
    std::string connect_string = "user=postgres password=postgres dbname=test";
    
    if (argc > 1)
    {
        connect_string += std::string("host=") + argv[1];
    }

    boost::asio::io_service fg_ios;
    boost::asio::io_service bg_ios;
    std::auto_ptr<boost::asio::io_service::work> fg_work(new boost::asio::io_service::work(fg_ios)); // this keeps the fg_ios alive 
    std::auto_ptr<boost::asio::io_service::work> bg_work(new boost::asio::io_service::work(bg_ios)); // this keeps the bg_ios alive
    boost::thread fg(boost::bind(&boost::asio::io_service::run, &fg_ios));
    boost::thread bg(boost::bind(&boost::asio::io_service::run, &bg_ios));

    // precondition CREATE TABLE postgres_asio_sample  ( id integer, val text )

    for (int i = 0; i != 50; ++i)
    {
        auto connection = boost::make_shared<postgres_asio::connection>(fg_ios, bg_ios);
        std::string trace_id = "xxxx-xxxx-xx" + std::to_string(i);
        connection->connect(connect_string, [connection](int ec)
        {
            if (!ec)
            {
                std::string statement = "insert into postgres_asio_sample (id, val) VALUES\n";
                size_t items = 100;
                for (int i = 0; i != items; ++i)
                {
                    const char* ch = i < (items - 1) ? ",\n" : ";\n";
                    std::string val = to_string(boost::uuids::random_generator()());
                    statement += " (" + std::to_string(i) + ", '" + val + "')" + ch;
                }

                BOOST_LOG_TRIVIAL(info) << "async_insert";
                connection->exec(statement, [connection](int ec, boost::shared_ptr<PGresult> res)
                {
                    if (ec)
                    {
                        BOOST_LOG_TRIVIAL(error) << " insert failed ec:" << ec << " last_error:" << connection->last_error();
                        return;
                    }
                    BOOST_LOG_TRIVIAL(info) << "async insert - done!!";
                });
            }
        });
    }

    BOOST_LOG_TRIVIAL(debug) << "work reset";
    bg_work.reset();
    BOOST_LOG_TRIVIAL(debug) << "bg join";
    bg.join();

    fg_work.reset();
    BOOST_LOG_TRIVIAL(debug) << "fg join";
    fg.join();
    BOOST_LOG_TRIVIAL(debug) << "done";
}