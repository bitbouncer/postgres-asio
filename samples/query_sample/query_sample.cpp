#include <iostream>
#include <postgres_asio/postgres_asio.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

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

    auto connection = boost::make_shared<postgres_asio::connection>(fg_ios, bg_ios);
    connection->connect(connect_string, [connection](int ec)
    {
        std::cerr << "connect_async : " << ec << std::endl;
        if (!ec)
        {
            connection->exec("select * from postgres_asio_sample;", [connection](int ec, boost::shared_ptr<PGresult> res)
            {
                if (ec)
                {
                    std::cerr << "select failed " << ec << std::endl;
                    return;
                }

                int nFields = PQnfields(res.get());
                for (int i = 0; i < nFields; i++)
                    printf("%-15s", PQfname(res.get(), i));

                printf("\n\n");
                printf("query returned %d rows", PQntuples(res.get()));

                //for (int i = 0; i < PQntuples(res.get()); i++)
                //{
                //    for (int j = 0; j < nFields; j++)
                //        printf("%-15s", PQgetvalue(res.get(), i, j));
                //    printf("\n");
                //}
                ;
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