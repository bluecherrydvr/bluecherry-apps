<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Monitoring Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body { font-family: Arial, sans-serif; margin: 2em; }
        #charts { display: flex; flex-wrap: wrap; gap: 2em; }
        .chart-container { width: 400px; }
    </style>
</head>
<body>
    <div class="row">
        <div class="col-lg-12">
            <h1 class="page-header">System Monitoring
                <ol class="breadcrumb">
                    <li class="active">Real-time system statistics</li>
                </ol>
            </h1>
        </div>
    </div>

    <div class="row">
        <div class="col-lg-12">
            <div class="alert alert-info">
                <i class="fa fa-info-circle"></i> 
                <strong>Server Status:</strong> 
                <?php if ($server_running): ?>
                    <span class="text-success">BlueCherry Server is running</span>
                <?php else: ?>
                    <span class="text-danger">BlueCherry Server is not running</span>
                <?php endif; ?>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="col-lg-6">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h3 class="panel-title"><i class="fa fa-tachometer fa-fw"></i> Current CPU Usage</h3>
                </div>
                <div class="panel-body">
                    <div class="progress">
                        <div class="progress-bar" role="progressbar" style="width: <?php echo $cpu_usage; ?>%;" 
                             aria-valuenow="<?php echo $cpu_usage; ?>" aria-valuemin="0" aria-valuemax="100">
                            <?php echo number_format($cpu_usage, 1); ?>%
                        </div>
                    </div>
                    <small class="text-muted">Based on system load average</small>
                </div>
            </div>
        </div>
        
        <div class="col-lg-6">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h3 class="panel-title"><i class="fa fa-memory fa-fw"></i> Current Memory Usage</h3>
                </div>
                <div class="panel-body">
                    <div class="progress">
                        <div class="progress-bar" role="progressbar" style="width: <?php echo $memory_usage; ?>%;" 
                             aria-valuenow="<?php echo $memory_usage; ?>" aria-valuemin="0" aria-valuemax="100">
                            <?php echo number_format($memory_usage, 1); ?>%
                        </div>
                    </div>
                    <small class="text-muted">System memory utilization</small>
                </div>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="col-lg-12">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h3 class="panel-title"><i class="fa fa-line-chart fa-fw"></i> Historical Data</h3>
                </div>
                <div class="panel-body">
                    <div id="charts">
                        <div class="row">
                            <div class="col-md-6">
                                <h4>CPU Usage Over Time</h4>
                                <canvas id="cpuChart" width="400" height="200"></canvas>
                            </div>
                            <div class="col-md-6">
                                <h4>Memory Usage Over Time</h4>
                                <canvas id="memoryChart" width="400" height="200"></canvas>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script>
    let chartInstances = {};

    function renderChart(canvasId, label, labels, data) {
        const ctx = document.getElementById(canvasId).getContext('2d');
        if (chartInstances[canvasId]) {
            chartInstances[canvasId].data.labels = labels;
            chartInstances[canvasId].data.datasets[0].data = data;
            chartInstances[canvasId].update();
        } else {
            chartInstances[canvasId] = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: label,
                        data: data,
                        borderColor: 'rgb(75, 192, 192)',
                        backgroundColor: 'rgba(75, 192, 192, 0.1)',
                        fill: true,
                        tension: 0.4
                    }]
                },
                options: {
                    responsive: true,
                    scales: { 
                        x: { display: true }, 
                        y: { 
                            display: true,
                            beginAtZero: true,
                            max: 100
                        } 
                    },
                    plugins: {
                        legend: {
                            display: false
                        }
                    }
                }
            });
        }
    }

    function fetchStatsAndRender() {
        fetch('/ajax/monitoring/stats.php')
            .then(res => {
                if (!res.ok) throw new Error('Not logged in or API error');
                return res.json();
            })
            .then(data => {
                renderChart('cpuChart', 'CPU Usage', data.timestamps, data.cpu);
                renderChart('memoryChart', 'Memory Usage', data.timestamps, data.memory);
            })
            .catch(err => {
                console.log('Monitoring data not available:', err.message);
            });
    }

    // Initialize charts with current data
    let currentTime = Math.floor(Date.now() / 1000);
    renderChart('cpuChart', 'CPU Usage', [currentTime], [<?php echo $cpu_usage; ?>]);
    renderChart('memoryChart', 'Memory Usage', [currentTime], [<?php echo $memory_usage; ?>]);

    // Update every 10 seconds
    setInterval(fetchStatsAndRender, 10000);
    </script>
</body>
</html> 