<?php
/**
 * Created by IntelliJ IDEA.
 * User: Filip
 * Date: 23/03/2018
 * Time: 17:36
 */

require 'mysql.php';

if (isset($_GET['device_id']) && isset($_GET['last_timestamp']) && isset($_GET['from_device'])) {
    $device_id = $_GET['device_id'];
    $timestamp = $_GET['last_timestamp'];
    $from_device = $_GET['from_device'];
    if ($timestamp === 'true') {
        $sql = "SELECT command_timestamp, command FROM petfeeder_commands WHERE
        device_id = $device_id AND from_device = $from_device ORDER BY command_timestamp DESC LIMIT 1;";
    } else {
        $sql = "SELECT command_timestamp, command FROM petfeeder_commands WHERE
        device_id = $device_id AND from_device = $from_device AND command_timestamp >= FROM_UNIXTIME($timestamp);";
    }
    $result = $mysql->query($sql);
    if ($result->num_rows > 0) {
        $output = array();
        while($row = mysqli_fetch_assoc($result)) {
            $output[] = $row;
        }
        print json_encode($output);
    } else {
        echo "ERROR: " . $sql . "<br>" . $mysql->error;
    }
} else {
    echo "ERROR: wrong parameters";
}