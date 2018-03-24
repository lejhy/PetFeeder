/* jshint -W097 */
"use strict";
/*
* Code for University of Strathclyde CX318.
* Developed by Filip Lejhanec 2018.
*/
function View() {

    var status = document.getElementById("status");
    var feed = document.getElementById("feed");
    var manualLeft = document.getElementById("manual-left");
    var manualRight = document.getElementById("manual-right");

    function setStatus(current, maximum) {
        status.innerHTML = current+"/"+maximum;
    }

    function setFeedListener(listener) {
        feed.onclick = listener;
    }

    function setManualLeftListener(listener) {
        manualLeft.onclick = listener;
    }

    function setManualRightListener(listener) {
        manualRight.onclick = listener;
    }

    return {
        setStatus: setStatus,
        setFeedListener: setFeedListener,
        setManualLeftListener: setManualLeftListener,
        setManualRightListener: setManualRightListener
    }
}
