$(function() {
    console.log("This loaded!");
});

function testCameraConnection(form, msg) {
    let data = JSON.parse(msg.data);

    $('#connStat').find('form').show();
    if(data.success)
        $('#connStat').removeClass().addClass('alert alert-success').find('span').html(msg.msg[1]);
    else 
        $('#connStat').removeClass().addClass('alert alert-danger').find('span').html(msg.msg[1]);
}

function testCameraLoadingHeader(form, msg) {
    $('#connStat').removeClass().addClass('alert alert-warning').find('span').html('Testing...');
    $('#connStat').find('form').hide();
}