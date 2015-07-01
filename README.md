# postgres-asio
=========

A C++11 asyncronous postgres client library based on boost::asio. The library provides a thin skin on top of the C-API pglib


Platforms: Windows / Linux

Building
see
https://github.com/bitbouncer/csi-build-scripts


Samples:
- insert
- query
- query w cursor

all samples uses hardcoded connect parameters "user=postgres password=postgres dbname=test" with an option to add host as first parameter on the commandline.

you also have to create the table beforehand.

"CREATE TABLE postgres_asio_sample  ( id integer, val text )"

