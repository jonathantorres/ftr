<?php

$ftp = ftp_connect('localhost', 9090, 10);

if (!$ftp) {
    echo("Could not connect to FTP server\n");
    exit(1);
}

if (!ftp_set_option($ftp, FTP_USEPASVADDRESS, false)) {
    echo("Could not set FTP option\n");
}

if (!ftp_login($ftp, "jt", "test")) {
    echo("Could not login to server\n");
    ftp_close($ftp);
    exit(1);
}

if (!ftp_chdir($ftp, "/")) {
    echo("Could not change directory\n");
    ftp_close($ftp);
    exit(1);
}

if (!ftp_pasv($ftp, true)) {
    echo("Could not set passive mode");
    ftp_close($ftp);
    exit(1);
}

if (!ftp_raw($ftp, "TYPE I")) {
    echo("Could not execute TYPE command");
    ftp_close($ftp);
    exit(1);
}

$dirs = ftp_raw($ftp, "LIST");

if (!$dirs) {
    echo("Could not run LIST\n");
    ftp_close($ftp);
    exit(1);
}

var_dump($dirs);
ftp_close($ftp);

?>
