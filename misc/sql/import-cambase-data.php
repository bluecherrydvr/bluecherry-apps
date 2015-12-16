<?php

$tables = <<<TABLES

DROP TABLE IF EXISTS "CAMERAS";
DROP TABLE IF EXISTS "manufacturers";


CREATE TABLE "manufacturers" (
  "id" integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "manufacturer" text(31) NOT NULL,
  "model_id" text(31) NOT NULL
);


CREATE TABLE "cameras" (
  "id" integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "api_id" text NOT NULL,
  "model" text NOT NULL,
  "manufacturer" integer NOT NULL,
  "jpeg_url" text NOT NULL,
  "mjpeg_url" text NOT NULL,
  "h264_url" text NOT NULL,
  "resolution" text NOT NULL,
  "shape" text NOT NULL,
  "default_username" text NOT NULL DEFAULT '',
  "default_password" text NOT NULL DEFAULT '',
  "psia" text NOT NULL DEFAULT '',
  "onvif" integer NOT NULL DEFAULT '0',
  "ptz" integer NOT NULL DEFAULT '0',
  "infrared" integer NOT NULL DEFAULT '0',
  "varifocal" integer NOT NULL DEFAULT '0',
  "sd_card" integer NOT NULL DEFAULT '0',
  "upnp" integer NOT NULL DEFAULT '0',
  "poe" integer NOT NULL DEFAULT '0',
  "audio_in" integer NOT NULL DEFAULT '0',
  "audio_out" integer NOT NULL DEFAULT '0',
  "discontinued" integer NOT NULL DEFAULT '0',
  "wifi" integer NOT NULL DEFAULT '0',
  "images" text NOT NULL DEFAULT '',
  "official_url" text NOT NULL,
  FOREIGN KEY ("manufacturer") REFERENCES "manufacturers" ("id") ON DELETE NO ACTION ON UPDATE NO ACTION
);


TABLES;

function sanitizeValues(&$value, $key) {
    if(in_array(
        $key, 
        array('jpeg_url', 'h264_url', 'mjpeg_url', 'default_username', 'default_password')
    )) {
        $value = str_replace(
            array('Unknown', 'unknown', '<blank>', 'blank', 'none', 'None', 'n/a', 'User defined', 'user defined'), 
            '', 
            $value
        );
    }
}

function print_help() {
echo "

HELP:

    Usage: command path-to-file [--create-tables | -c]

    path-to-file must be writable
    --create-tables | -c is an optional argument, when provided table creation statement will be prepend to file

";
die;
}

function processManufacturers($file){
    $base_url = 'http://api.cambase.io/api/v1/vendors.json?page=%d';
    $page = 1;
    $manufacturers = array();
    $i = 1;
    file_put_contents($file, PHP_EOL . '-- MANUFACTURERS --' . PHP_EOL, FILE_APPEND);
    do {
        $url = sprintf($base_url, $page);
        $data = @file_get_contents($url);
        if(!$data) {
            break;
        } 
        $data = json_decode($data, true);
        foreach($data['data']['vendors'] as $manufacturer) {
            $manufacturers[$manufacturer['id']] = $i;
            $sql = sprintf("INSERT INTO \"manufacturers\" VALUES ('%d', '%s', '%s');", $i, $manufacturer['name'], $manufacturer['id']);
            file_put_contents($file, $sql . PHP_EOL, FILE_APPEND);
            $i++;
        }
        $page++;
    } while($page <= $data['data']['paging']['number_of_pages']);
    return $manufacturers;
}

function processCams($file, $manufacturers){
    $base_url = 'http://api.cambase.io/api/v1/models.json?page=%d';
    $base_cam_url  = 'http://api.cambase.io/api/v1/models/%s.json';
    $page = 1;
    $i = $duplicated = $failed = 0;
    file_put_contents($file, PHP_EOL . '-- CAMERAS --' . PHP_EOL, FILE_APPEND);
    $ids = array(); 
    do {
        $url = sprintf($base_url, $page);
        $data = @file_get_contents($url);
        if(!$data) {
            break;
        } 
        $data = json_decode($data, true);

        if($i % 100 == 0) {
            echo PHP_EOL;
            echo sprintf(
                'Processing items %d-%d of %d.', 
                $i + 1, 
                $i + 100 > $data['data']['paging']['total_items'] ? $data['data']['paging']['total_items'] : ($i + 100), 
                $data['data']['paging']['total_items']
            );
            echo PHP_EOL;
        } 
        
        foreach($data['data']['models'] as $camera) {
            $i++;
            $cam_url = sprintf($base_cam_url, urlencode($camera['id']));
            $camData = @file_get_contents($cam_url);
            if(!$camData) {
                file_put_contents(
                    './log.txt', 
                    date('Y-m-d H:i:s') . ' ERROR  : (Cam "' . $camera['id'] . '" details could not be retrieved)' . PHP_EOL, 
                    FILE_APPEND
                );
                echo 'F';
                $failed++;
                continue;
            }
            $camData = json_decode($camData, true);
            $cam = $camData['models'];
            if(in_array($cam['id'], $ids)) {
                file_put_contents(
                    './log.txt', 
                    date('Y-m-d H:i:s') . ' NOTICE : (Duplicated entry : Cam "' . $camera['id'] . '")' . PHP_EOL, 
                    FILE_APPEND
                );
                echo 'D';
                $duplicated++;
                continue;
            }
            array_walk($cam, 'sanitizeValues');
            $sql = <<<SQL
INSERT INTO "cameras" 
VALUES (NULL, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s');
SQL;
            $sql = sprintf($sql, $cam['id'], $cam['model'], $manufacturers[$cam['vendor_id']], $cam['jpeg_url'], $cam['mjpeg_url'], 
                           $cam['h264_url'], $cam['resolution'], $cam['shape'], $cam['default_username'], $cam['default_password'], 
                           empty($cam['psia']) ? '' : $cam['psia'], $cam['onvif'], $cam['ptz'], $cam['infrared'], $cam['varifocal'], 
                           $cam['sd_card'], $cam['upnp'], $cam['poe'], $cam['audio_in'], $cam['audio_out'], $cam['discontinued'], 
                           $cam['wifi'], serialize($cam['images']), $cam['official_url']);
            file_put_contents($file, $sql . PHP_EOL, FILE_APPEND);
            echo '.';
            $ids[] = $cam['id'];
        }
        $page++;
    } while($page <= $data['data']['paging']['number_of_pages']);

    echo PHP_EOL . PHP_EOL;
    echo '-=-=-=-=-= IMPORT SUMMARY =-=-=-=-=-' . PHP_EOL;
    echo $data['data']['paging']['total_items'] . ' items were processed.' . PHP_EOL;
    echo $failed . ' items failed to import.' . PHP_EOL;
    echo $duplicated . ' duplicated items were found.' . PHP_EOL . PHP_EOL; 

}

if(count($argv) == 1) {
    print_help();    
}

array_shift($argv);

if(in_array('--help', $argv) || in_array('-h', $argv)) {
    print_help();
}

$file = array_shift($argv);

if(!is_writeable($file) && !touch($file)) {
    echo '

    ERROR: File ' . realpath($file) . ' is not writeable. Please choose another file / location.


';
die;
}

echo PHP_EOL . "You are about to import Cambase data into a local dump file.";
echo PHP_EOL . "NOTE: This is a long operation, it can take several minutes.";
echo PHP_EOL . "Press [ENTER] key to start ... Press CTRL-C To exit.";

fgets(STDIN);

echo PHP_EOL . "Importing new data to " . realpath($file) . "." . PHP_EOL;

if(in_array('--create-tables', $argv) || in_array('-c', $argv)){
    file_put_contents($file, $tables);
}

$manufacturers = processManufacturers($file);

echo PHP_EOL;

processCams($file, $manufacturers);

echo PHP_EOL;
