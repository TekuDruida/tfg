#!/bin/bash

tfg/libsvm-3.21/svm-predict tfg/inputs/featuresCoded/fcDev$3.txt tfg/outputs/models/train$3-$1-$2.model tfg/outputs/predictions/$3-$1-$2_predict.txt > tfg/outputs/accuracy/$3-$1-$2_acc.txt
