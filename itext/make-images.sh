#!/bin/sh
$1 /dev/null fressi.punch fressi.img >fressi.out
$1 /dev/null charsub1.punch charsub1.img >charsub1.out
$1 /dev/null nehspech.punch nehspech.img >nehspec.out
$1 charsub1.img fressi.punch cf.img
$1 charsub1.img nehspech.punch cn.img
$1 cn.img fressi.punch cnf.img
$1 fressi.img charsub1.punch fc.img
