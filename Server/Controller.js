/* jshint -W097 */
"use strict";
/*
* Code for University of Strathclyde CX318.
* Developed by Filip Lejhanec 2018.
*/

function Controller(model, view) {

    function init() {
        model.addObserver(this);
        model.startInterval();

        view.setFeedListener(model.feed);
        view.setManualLeftListener(model.left);
        view.setManualRightListener(model.right);
    }

    function update() {
        view.setStatus(model.getCurrent(), model.getMaximum());
    }

    return {
        init : init,
        update: update
    }
}
