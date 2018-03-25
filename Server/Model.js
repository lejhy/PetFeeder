/* jshint -W097 */
"use strict";
/*
* Code for University of Strathclyde CX318.
* Developed by Filip Lejhanec 2018.
*/
function Model() {

    var observers = [];

    var id = 1;
    var current;
    var maximum;
    var interval = 0;

    function feed() {
        command("feed");
    }

    function left() {
        command("left");
    }

    function right() {
        command("right");
    }

    function command(command) {
        var request = "http://"+document.location.host+"/post.php?command="+command+"&device_id="+id+"&from_device=false";
        var callback = function(responseText) {
            var response = JSON.parse(responseText);
            if (response.length == 1) {
                var command = JSON.parse(response[0].command);
                current = command.current;
                maximum = command.maximum;
                notify();
            } else {
                console.log("ERROR: Should have received only one row");
            }
        };
        ajax(request, callback);
    }

    function startInterval() {
        if (interval === 0) {
            interval = setInterval(refresh, 3000);
        }
    }

    function stopInterval() {
        if (interval !== 0) {
            clearInterval(interval);
            interval = 0;
        }
    }

    function refresh() {
        var request = "http://"+document.location.host+"/get.php?last_timestamp=true&device_id="+id+"&from_device=true";
        var callback = function(responseText) {
            var response = JSON.parse(responseText);
            if (response.length == 1) {
                var command = JSON.parse(response[0].command);
                current = command.current;
                maximum = command.maximum;
                notify();
            } else {
                console.log("ERROR: Should have received only one row");
            }
        };
        ajax(request, callback);
    }

    function ajax(request, callback) {
        var ajax = new XMLHttpRequest();
        ajax.onreadystatechange = function() {
            if (this.readyState === 4 && this.status === 200) {
                callback(this.responseText);
            }
        };
        ajax.open("GET", request, true);
        ajax.send();
    }
    function getCurrent() {
        return current;
    }

    function getMaximum() {
        return maximum;
    }

    function addObserver(observer) {
        observers.push(observer);
    }

    function removeObserver(observer) {
        var index = observers.indexOf(observer);
        observers.splice(index, 1);
    }

    function notify(args) {
        observers.forEach(function(observer) {
            observer.update(args);
        });
    }

    return {
        feed: feed,
        left: left,
        right: right,
        startInterval: startInterval,
        stopInterval: stopInterval,
        getCurrent: getCurrent,
        getMaximum: getMaximum,
        addObserver: addObserver,
        removeObserver: removeObserver
    }
}
