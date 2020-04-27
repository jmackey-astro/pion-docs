#!/bin/bash

rsync -uvrt --delete build/html/ ariadne:public_html/pion_documentation/

