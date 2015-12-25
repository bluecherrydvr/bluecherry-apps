$(function() {

});

function playbackSetVideo(el) {
    var parent = $('#playback-player-bl');
    var player_bl = $('#playback-player');
    var id = el.data('id');
    var media_id = el.data('media-id');
    var url = '/media/stream-mp4?id='+id;
    var poster = '/media/request?mode=screenshot&device_id='+id+'&id='+media_id;


    // set width/height
    var player_old = player_bl.find('video');
    if (player_old.length) {
        if (parseInt(player_old.width()) != 100) {
            player_bl.css('width', player_old.width());
            player_bl.css('height', player_old.height());
        }
    }

    $('#playback-events-list tr').removeClass('selected');
    el.addClass('selected');
    $('#playback-player-download').attr('href', url+'&download=1');

    parent.find('.panel-heading').html(el.data('title'));

    player_bl.html('<video src="'+url+'" type="video/mp4" poster="'+poster+'" style="width: 100%; height: 100%;" preload="none" controls></video>');
    var player = player_bl.find('video');


    parent.show();

    player.mediaelementplayer();
}

function playbackReloadVideo(el) {
    var player_bl = $('#playback-player');
    var player = player_bl.find('video').clone();

    player_bl.html(player);
    player.mediaelementplayer();
}

function playbackSetResult(form, msg) {
    $('#playback-results').html(msg);
}

function playbackSetDate(el) {
    var start_date = '';
    var end_date = '';
    var date = new Date();
    var form = $('#playback-form-search');

    if (el == null) {
        var val = 'today';
    } else {
        var val = el.val();
    }

    if (val == 'today') {
        start_date = date;
        end_date = date;
    } else if (val == 'yesterday') {
        var date_tmp = new Date(date.setDate(date.getDate() -1));

        start_date = date_tmp;
        end_date = date_tmp;
    } else if (val == 'this_week') {
        var first_day = date.getDate() - date.getDay();
        var last_day = first_day + 6;

        start_date = new Date(date.setDate(first_day));
        end_date = new Date(date.setDate(last_day));
    } else {
        // last week
        var first_day = (date.getDate() - 7) - date.getDay();
        var last_day = first_day + 6;

        start_date = new Date(date.setDate(first_day));
        end_date = new Date(date.setDate(last_day));
    }

    if (var_locale_en) {
        start_date = ('0' + (start_date.getMonth() + 1)).slice(-2) + '/' + ('0' + start_date.getDate()).slice(-2) + '/' + start_date.getFullYear() + ' 00:00 AM';
        end_date = ('0' + (end_date.getMonth() + 1)).slice(-2) + '/' + ('0' + end_date.getDate()).slice(-2) + '/' + end_date.getFullYear() + ' 11:59 PM';
    } else {
        start_date = ('0' + start_date.getDate()).slice(-2) + '.' + ('0' + (start_date.getMonth() + 1)).slice(-2) + '.' + start_date.getFullYear() + ' 00:00';
        end_date = ('0' + end_date.getDate()).slice(-2) + '.' + ('0' + (end_date.getMonth() + 1)).slice(-2) + '.' + end_date.getFullYear() + ' 23:59';
    }

    form.find('input[name="start"]').val(start_date);
    form.find('input[name="end"]').val(end_date);
}
