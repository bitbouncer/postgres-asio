#include <iostream>
#include <thread>
#include <postgres_asio/postgres_asio.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

int main(int argc, char *argv[]) {
  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  std::string connect_string = "user=postgres password=postgres dbname=test";

  if(argc > 1) {
    connect_string += std::string("host=") + argv[1];
  }

  boost::asio::io_service fg_ios;
  boost::asio::io_service bg_ios;
  auto fg_work(std::make_unique<boost::asio::io_service::work>(fg_ios)); // this keeps the fg_ios alive
  auto bg_work(std::make_unique<boost::asio::io_service::work>(bg_ios)); // this keeps the bg_ios alive
  std::thread fg([&] { fg_ios.run(); });
  std::thread bg([&] { bg_ios.run(); });

  auto connection = std::make_shared<postgres_asio::connection>(fg_ios, bg_ios);
  connection->connect(connect_string, [connection](int ec) {
    BOOST_LOG_TRIVIAL(info) << "connect_async ec : " << ec;
    if(!ec) {
      BOOST_LOG_TRIVIAL(info) << "calling select_async";
      connection->exec("select * from postgres_asio_sample;", [connection](int ec, std::shared_ptr<PGresult> res) {
        if(ec) {
          BOOST_LOG_TRIVIAL(error) << "select failed, ec: " << ec;
          return;
        }

        BOOST_LOG_TRIVIAL(info) << "select_async returned " << PQntuples(res.get()) << " rows";

        //int nFields = PQnfields(res.get());
        //for (int i = 0; i < nFields; i++)
        //    printf("%-15s", PQfname(res.get(), i));

        //printf("\n\n");
        //printf("query returned %d rows", PQntuples(res.get()));

        //for (int i = 0; i < PQntuples(res.get()); i++)
        //{
        //    for (int j = 0; j < nFields; j++)
        //        printf("%-15s", PQgetvalue(res.get(), i, j));
        //    printf("\n");
        //}
      });
    }
  });

  BOOST_LOG_TRIVIAL(debug) << "work reset";
  bg_work.reset();
  BOOST_LOG_TRIVIAL(debug) << "bg join";
  bg.join();

  fg_work.reset();
  BOOST_LOG_TRIVIAL(debug) << "fg join";
  fg.join();
  BOOST_LOG_TRIVIAL(debug) << "done";
}