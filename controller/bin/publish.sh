#!/bin/bash

rsync -avz $(dirname $0)/../../controller/ yun02.ss:/root/datalamp

