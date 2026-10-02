<?php
// Focused regression test for the media/download helpers in www/lib/Helpers.php
// (issues #763, #764). Run from the repo root: php test_media_helpers.php

require_once 'www/lib/Helpers.php';

$failures = 0;
function check($name, $cond) {
    global $failures;
    if ($cond) {
        echo "ok - {$name}\n";
    } else {
        $failures++;
        echo "NOT OK - {$name}\n";
    }
}

// --- ffprobe fixtures (flat format) ---

$h264_first = 'streams.stream.0.index=0' . "\n"
    . 'streams.stream.0.codec_name="h264"' . "\n"
    . 'streams.stream.0.codec_type="video"' . "\n"
    . 'streams.stream.1.index=1' . "\n"
    . 'streams.stream.1.codec_name="aac"' . "\n"
    . 'streams.stream.1.codec_type="audio"' . "\n";

$audio_first = 'streams.stream.0.index=0' . "\n"
    . 'streams.stream.0.codec_name="pcm_mulaw"' . "\n"
    . 'streams.stream.0.codec_type="audio"' . "\n"
    . 'streams.stream.1.index=1' . "\n"
    . 'streams.stream.1.codec_name="h264"' . "\n"
    . 'streams.stream.1.codec_type="video"' . "\n";

$hevc_only = 'streams.stream.0.codec_name="hevc"' . "\n"
    . 'streams.stream.0.codec_type="video"' . "\n";

$no_video = 'streams.stream.0.codec_name="mp3"' . "\n"
    . 'streams.stream.0.codec_type="audio"' . "\n";

// --- ffprobeParseStreams ---

$s = ffprobeParseStreams($h264_first);
check('parse h264-first yields 2 streams', count($s) === 2);
check('parse keeps video codec at index 0', $s[0]['codec_type'] === 'video' && $s[0]['codec_name'] === 'h264');
check('parse keeps audio codec at index 1', $s[1]['codec_type'] === 'audio' && $s[1]['codec_name'] === 'aac');

check('parse ignores empty string', ffprobeParseStreams('') === array());
check('parse ignores null', ffprobeParseStreams(null) === array());
check('parse ignores garbage', ffprobeParseStreams("some error\nno streams here\n") === array());
check('parse lowercases values', ffprobeParseStreams("streams.stream.0.codec_type=\"Video\"\n") === array(0 => array('codec_type' => 'video', 'codec_name' => '')));

// --- ffprobeFirstVideoStream ---

$v = ffprobeFirstVideoStream(ffprobeParseStreams($h264_first));
check('video-first file finds video at index 0', $v !== null && $v['index'] === 0 && $v['codec'] === 'h264');

$v = ffprobeFirstVideoStream(ffprobeParseStreams($audio_first));
check('audio-first file finds video at index 1', $v !== null && $v['index'] === 1 && $v['codec'] === 'h264');

$v = ffprobeFirstVideoStream(ffprobeParseStreams($hevc_only));
check('hevc file finds video codec hevc', $v !== null && $v['index'] === 0 && $v['codec'] === 'hevc');

check('audio-only file yields no video stream', ffprobeFirstVideoStream(ffprobeParseStreams($no_video)) === null);
check('empty output yields no video stream', ffprobeFirstVideoStream(ffprobeParseStreams('')) === null);

// --- ffprobeHasStream ---

$s = ffprobeParseStreams($h264_first);
check('detects audio stream', ffprobeHasStream($s, 'audio') === true);
check('detects aac audio', ffprobeHasStream($s, 'audio', 'aac') === true);
check('rejects non-present audio codec', ffprobeHasStream($s, 'audio', 'mp3') === false);
check('rejects video-as-audio', ffprobeHasStream($s, 'subtitle') === false);

$s = ffprobeParseStreams($hevc_only);
check('video-only file has no audio', ffprobeHasStream($s, 'audio') === false);

// --- downloadFileError ---

$err = downloadFileError('/nonexistent-dir-xyz/missing.mkv', 'missing.mkv');
check('missing file yields 404', $err !== null && $err[0] === 404);
check('missing file message names the file', $err !== null && strpos($err[1], 'missing.mkv') !== false);

$tmp = tempnam(sys_get_temp_dir(), 'bctest_');
file_put_contents($tmp, 'data');
check('readable file yields no error', downloadFileError($tmp, 'recording.mkv') === null);

$err = downloadFileError('/nonexistent-dir-xyz/missing.mkv');
check('missing temp file uses fallback label', $err !== null && strpos($err[1], 'temporary transcoded file') !== false);

if (function_exists('posix_getuid') && posix_getuid() === 0) {
    echo "skip - unreadable file check (running as root bypasses permissions)\n";
} else {
    chmod($tmp, 0000);
    $readable_after_chmod = is_readable($tmp);
    if ($readable_after_chmod) {
        echo "skip - unreadable file check (chmod 000 did not remove readability here)\n";
    } else {
        $err = downloadFileError($tmp, 'recording.mkv');
        check('unreadable file yields 500', $err !== null && $err[0] === 500);
        check('unreadable message hints at permissions', $err !== null && strpos($err[1], 'permissions') !== false);
    }
    chmod($tmp, 0600);
}
unlink($tmp);

if ($failures > 0) {
    echo "\n{$failures} check(s) FAILED\n";
    exit(1);
}
echo "\nall checks passed\n";
