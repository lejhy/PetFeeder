<?php
/**
 * Created by IntelliJ IDEA.
 * User: flejh
 * Date: 22/03/2018
 * Time: 15:26
 */

require 'mysql.php';

if (isset($_GET['device_id']) && isset($_GET['command']) && isset($_GET['from_device'])) {
    $device_id = $_GET['device_id'];
    $command = $_GET['command'];
    $from_device = $_GET['from_device'];
    $sql = "INSERT INTO petfeeder_commands (command_timestamp, device_id, command, from_device)
        VALUES (CURRENT_TIMESTAMP, $device_id, '$command', $from_device)";
    if ($mysql->query($sql) === TRUE) {
        echo "OK";
    } else {
        echo "ERROR: " . $sql . "<br>" . $mysql->error;
    }
} else {
    echo "ERROR: wrong parameters";
}

