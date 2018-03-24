<!DOCTYPE html>
<!--
 Code for University of Strathclyde CX318.
 Developed by Filip Lejhanec 2018.
-->
<html>
<head>
    <title>Pet Feeder</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no" />
    <meta name="mobile-web-app-capable" content="yes" />
    <meta name="apple-mobile-web-app-capable" content="yes" />

    <link rel="icon" sizes="192x192" href="icon.png" />
    <link rel="apple-touch-icon" href="icon.png" />
    <link rel="shortcut icon" href="weeicon.png" type="image/x-icon" />

    <link rel="stylesheet" type="text/css" href="normalize.css" />
    <link rel="stylesheet" type="text/css" href="style.css" />
</head>
<body>
<header>Pet Feeder</header>
<main>
    <div id="status"></div>
    <div id="status-description">crickets left</div>
    <div id="controls">
        <div id="feed">FEED</div>
        <div id="manual-left"></div>
        <div id="manual-right"></div>
    </div>
</main>
<footer>&copy; Group 41, 2018</footer>
<script src="Model.js"></script>
<script src="View.js"></script>
<script src="Controller.js"></script>
<script src="Main.js"></script>
</body>
</html>

<?php
/**
 * Created by IntelliJ IDEA.
 * User: flejh
 * Date: 22/03/2018
 * Time: 15:21
 */

require "mysql.php";


$sql = "SELECT * FROM petfeeder_commands";
$result = $mysql->query($sql);
echo var_dump($result);