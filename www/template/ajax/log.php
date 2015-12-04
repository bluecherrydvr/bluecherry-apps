<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"></h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <a href="/downloadlogs" class="btn btn-success pull-right"><?php echo LOG_DOWNLOAD; ?></a>
        <div class="clearfix"></div>
    </div>
</div>
<br>

<div class="row">
    <div class="col-lg-12">
        <div class="panel panel-default">
            <div class="panel-body" id="log-info-bl">
            <table class="table table-striped">
            <?php echo $log_html; ?>
            </table>
            </div>
        </div>
    </div>
</div>

<div class="row">
    
    <form action="/log" method="post">
    <div class="col-lg-4 col-md-4 form-group">
	    <?php echo arrayToSelect(array('bc' => 'Bluecherry Server Log', 'www' => 'Bluecherry WWW Log'), $type, 'type', 'send-req-form-select', false, 'data-type-data="HTML" data-func-after="logDispData"'); ?>
    </div>
    <div class="col-lg-4 col-md-4 form-group">
        <select name="lines" class="form-control send-req-form-select" data-type-data="HTML" data-func-after="logDispData">
            <option value="20"><?php echo LOG_HOW_MANY; ?></option>
            <option value="20">20</option>
            <option value="100">100</option>
            <option value="500">500</option>
            <option value="1000">1000</option>
            <option value="all"><?php echo LOG_ALL.": {$log_size}".(is_int($log_size) ? 'kb' : ''); ?></option>
        </select>
    </div>
    <div class="col-lg-4 col-md-4 form-group">
        <div class="btn-group">
    	    <a href="javascript:void(0);" class="btn btn-primary click-event" data-class="logControl.scrollBottom()"><i class="fa fa-arrow-circle-down fa-fw"></i> <?php echo SCROLL_TO_BOTTOM; ?></a>
            <a href="javascript:void(0);" class="btn btn-primary click-event" data-class="logControl.scrollTop()"><i class="fa fa-arrow-circle-up fa-fw"></i> <?php echo SCROLL_TO_TOP; ?></a>
        </div>
    </div>
    </form>
</div>

<?php

addJs('
    $(function() {
        var lc = new logControl();
        lc.scrollBottom();
    });
');

?>
