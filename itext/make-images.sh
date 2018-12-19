#!/bin/sh
./a.out /dev/null fressi.punch fressi.img
./a.out /dev/null charsub1.punch charsub1.img
./a.out /dev/null nehspech.punch nehspech.img
./a.out charsub1.img fressi.punch cf.img
./a.out charsub1.img nehspech.punch cn.img
./a.out cn.img fressi.punch cnf.img
./a.out fressi.img charsub1.punch fc.img
