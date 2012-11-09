#!/bin/bash

mysqladmin create postest
if ( -f ) 
mysqladmin -f drop postest

