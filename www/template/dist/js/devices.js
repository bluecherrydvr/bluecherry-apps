$(function() {

    
    $('body').on("click", "#edittip-advanced-settings", function(e){
        var el = $('#edittip-advanced-settings-block');
        if (el.is(':visible')) el.hide();
        else el.show();
    });

    $('body').on("mousedown", ".table-grid td", function(e){
        $('body').on("mousemove", ".table-grid td", function(e){
            $(this).css('background-color', 'white');
        }).on('mouseup', function(e) {
            $('body').off("mousemove", ".table-grid td");
        });
    });

    $('body').on("click", ".table-grid td", function(e){
        $(this).css('background-color', 'white');
    });
    
});

var motionGrid = function(el) {
    var self = this;
    var grid_bl = null;
    var table_grid = null;
    var grid_color = null;

    var gridChanges = function () {
        table_grid.removeClass('bg-danger');
        table_grid.removeClass('bg-warning');
        table_grid.removeClass('bg-primary');
        table_grid.removeClass('bg-info');
        table_grid.removeClass('bg-success');
        table_grid.removeClass('bg-default');

        table_grid.addClass(grid_color);
    };

    var btnChanges = function () {
        var btns = $('.motion-btn-sens');
        btns.addClass('disabled');
        el.removeClass('disabled');
    }

    self.off = function () {
        grid_color = 'bg-default';
        el.data('active', 'default');
        btnChanges();
    };

    self.minimal = function () {
        grid_color = 'bg-success';
        el.data('active', 'success');
        btnChanges();
    };

    self.low = function () {
        grid_color = 'bg-info';
        el.data('active', 'info');
        btnChanges();
    };

    self.average = function () {
        grid_color = 'bg-primary';
        el.data('active', 'primary');
        btnChanges();
    };

    self.high = function () {
        grid_color = 'bg-warning';
        el.data('active', 'warning');
        btnChanges();
    };

    self.veryHigh = function () {
        grid_color = 'bg-danger';
        el.data('active', 'danger');
        btnChanges();
    };

    self.showHideImage = function () {
        var img = grid_bl.find('img');
        if (img.is(':visible')) img.hide();
        else img.show();
    };
    
    self.clearAll = function () {
        grid_color = 'bg-default';
        gridChanges();
    };
    
    self.fillAll = function () {
        var btn = $('.motion-sens-bl').find('button:not(.disabled)');
        grid_color = 'bg-'+btn.data('active');
        gridChanges();
    };

    self.drawGrid = function () {
        var img = grid_bl.find('img');

        var width = img.width();
        var height = img.height();
    	var numcols = width/16;
        var numrows = height/16;

        var grid_table = '<table class="table-grid bg-primary" border="1" style="width: '+width+'px; height: '+height+'px;">';

    	for (row = 1; row<=numrows; row++) {
            grid_table += '<tr>';
            for(col = 1; col<=numcols; col++) {
                grid_table += '<td data-type="3">';
                grid_table += '</td>';
            }
            grid_table += '</tr>';
        };

        grid_table += '<table>';
        grid_bl.append(grid_table);
    };

    var constructor = function () {
        grid_bl = $('.grid-bl');
        table_grid = $('.table-grid');
    };
    constructor();
}

function delCamera(form) {
    form.closest('.well').remove();
}

function deviceSwith(form, msg) {
    // okay
    if (msg.status == 6) {
        var par = form.closest('.well');
        var icon = par.find('.devices-device-name i');
        var submit = par.find('button.devices-onoff-form');

        if (icon.hasClass('text-status-OK')) {
            icon.removeClass('text-status-OK');
            icon.addClass('text-status-disabled');
            submit.data('complete-text', $('#devices-text-status-disabled').text());
        } else {
            icon.removeClass('text-status-disabled');
            icon.addClass('text-status-OK');
            submit.data('complete-text', $('#devices-text-status-OK').text());
        }
    } else {
        // error

    }
}
