﻿#include "QComboBoxMoreSignal.h"

QComboBoxMoreSignal::QComboBoxMoreSignal(QWidget *parent):
    QComboBox(parent){
}

void QComboBoxMoreSignal::showPopup(){
    emit combo_box_showpopup(this);
    QComboBox::showPopup();
}
