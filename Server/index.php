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
    <div id="status">
        <?php

            require "mysql.php";

            $sql = "SELECT UNIX_TIMESTAMP(command_timestamp) AS command_timestamp, command FROM petfeeder_commands WHERE
            device_id = 1 AND from_device = true ORDER BY command_timestamp DESC LIMIT 1;";
            $result = $mysql->query($sql);
            if ($result->num_rows > 0) {
                $row = mysqli_fetch_assoc($result);
                $command = json_decode($row['command']);
                echo $command->current . "/" . $command->maximum;
            } else {
                echo "0/0";
            }

        ?>
    </div>
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
