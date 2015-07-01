#include <iostream>
#include <postgres_asio/postgres_asio.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>


void handle_fetch1000(boost::shared_ptr<postgres_asio::connection> connection, size_t total_count, int ec, boost::shared_ptr<PGresult> res)
{
    if (ec)
        return;
    
    int tuples_in_batch = PQntuples(res.get());
    total_count += tuples_in_batch;
    std::cerr << "got " << tuples_in_batch << ", total: " << total_count << std::endl;
    if (tuples_in_batch == 0)
    {
        connection->exec("CLOSE mycursor; COMMIT", [connection](int ec, boost::shared_ptr<PGresult> res) { });
        return;
    }
    connection->exec("FETCH 1000 in mycursor", [connection, total_count](int ec, boost::shared_ptr<PGresult> res) { handle_fetch1000(connection, total_count, ec, std::move(res)); });
}

int main(int argc, char *argv[])
{
    std::string host;
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

    auto connection = boost::make_shared<postgres_asio::connection>(fg_ios, bg_ios);
    connection->set_warning_timout(100);
    connection->connect(connect_string, [connection](int ec)
    {
        if (!ec)
        {
            connection->exec("BEGIN", [connection](int ec, boost::shared_ptr<PGresult> res)
            {
                if (ec)
                {
                    std::cerr << "BEGIN failed ec:" << ec << " last_error: " << connection->last_error() << std::endl;
                    return;
                }
                connection->exec("DECLARE mycursor CURSOR FOR SELECT * from postgres_asio_sample", [connection](int ec, boost::shared_ptr<PGresult> res)
                {
                    if (ec)
                    {
                        std::cerr << "DECLARE mycursor... failed ec:" << ec << " last_error: " << connection->last_error() << std::endl;
                        return;
                    }
                    connection->exec("FETCH 1000 in mycursor", [connection](int ec, boost::shared_ptr<PGresult> res){ handle_fetch1000(connection, 0, ec, std::move(res)); });
                });
            });
        }
    });

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