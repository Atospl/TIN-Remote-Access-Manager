#!/bin/sh
mkdir poco
wget http://pocoproject.org/releases/poco-1.7.3/poco-1.7.3-all.tar.gz
mv poco-1.7.3-all.tar.gz poco/
cd poco
tar -xzf poco-1.7.3-all.tar.gz
cd poco-1.7.3-all
sudo ./configure --prefix=/usr/
sudo make -s -j 2
sudo make install
