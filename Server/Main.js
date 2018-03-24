/* jshint -W097 */
"use strict";
/*
* Code for University of Strathclyde CX318.
* Developed by Filip Lejhanec 2018.
*/

var model;
var view;
var controller;

function init() {
    model = new Model();
    view = new View();
    controller = new Controller(model, view);
    controller.init();
}

window.addEventListener("load", init);