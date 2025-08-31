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
        #historical-chart { 
            position: relative; 
            height: 400px; 
            background: #f8f9fa; 
            border-radius: 8px; 
            padding: 20px; 
            margin-top: 10px;
        }
        .panel { margin-bottom: 20px; }
        .huge { font-size: 2.5em; font-weight: bold; }
        .time-range-btn.active { background-color: #337ab7; color: white; }
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
                <span id="server-status">Checking...</span>
                <button type="button" class="btn btn-sm btn-default pull-right" onclick="toggleDebug()">
                    <i class="fa fa-bug"></i> Debug
                </button>
            </div>
        </div>
    </div>
    
    <div class="row" id="debug-panel" style="display: none;">
        <div class="col-lg-12">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h3 class="panel-title">Debug Information</h3>
                </div>
                <div class="panel-body">
                    <div id="debug-info">
                        <p>Click "Debug" to show detailed information about the monitoring system.</p>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="col-lg-12">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h3 class="panel-title">
                        <i class="fa fa-clock-o"></i> Time Range
                    </h3>
                </div>
                <div class="panel-body">
                    <div class="btn-group" role="group">
                        <button type="button" class="btn btn-default time-range-btn" data-range="1h">Last Hour</button>
                        <button type="button" class="btn btn-default time-range-btn" data-range="6h">Last 6 Hours</button>
                        <button type="button" class="btn btn-default time-range-btn" data-range="1d">Last Day</button>
                        <button type="button" class="btn btn-default time-range-btn" data-range="1w">Last Week</button>
                        <button type="button" class="btn btn-default time-range-btn" data-range="1m">Last Month</button>
                    </div>
                    <span id="rrd-status" class="pull-right"></span>
                </div>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="col-lg-4">
            <div class="panel panel-primary">
                <div class="panel-heading">
                    <div class="row">
                        <div class="col-xs-3">
                            <i class="fa fa-microchip fa-5x"></i>
                        </div>
                        <div class="col-xs-9 text-right">
                            <div class="huge" id="cpu-usage">--</div>
                            <div>CPU Usage</div>
                        </div>
                    </div>
                </div>
                <div class="panel-footer">
                    <span class="pull-left">Current</span>
                    <span class="pull-right"><i class="fa fa-arrow-circle-right"></i></span>
                    <div class="clearfix"></div>
                </div>
            </div>
        </div>
        <div class="col-lg-4">
            <div class="panel panel-green">
                <div class="panel-heading">
                    <div class="row">
                        <div class="col-xs-3">
                            <i class="fa fa-memory fa-5x"></i>
                        </div>
                        <div class="col-xs-9 text-right">
                            <div class="huge" id="memory-usage">--</div>
                            <div>Memory Usage</div>
                        </div>
                    </div>
                </div>
                <div class="panel-footer">
                    <span class="pull-left">Current</span>
                    <span class="pull-right"><i class="fa fa-arrow-circle-right"></i></span>
                    <div class="clearfix"></div>
                </div>
            </div>
        </div>
        <div class="col-lg-4">
            <div class="panel panel-yellow">
                <div class="panel-heading">
                    <div class="row">
                        <div class="col-xs-3">
                            <i class="fa fa-hdd-o fa-5x"></i>
                        </div>
                        <div class="col-xs-9 text-right">
                            <div class="huge" id="disk-usage">--</div>
                            <div>Disk Usage</div>
                        </div>
                    </div>
                </div>
                <div class="panel-footer">
                    <span class="pull-left">Current</span>
                    <span class="pull-right"><i class="fa fa-arrow-circle-right"></i></span>
                    <div class="clearfix"></div>
                </div>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="col-lg-12">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h3 class="panel-title"><i class="fa fa-network-wired"></i> Network Interface Status</h3>
                </div>
                <div class="panel-body">
                    <div id="network-interfaces" class="row">
                        <!-- Network interface panels will be dynamically generated here -->
                    </div>
                </div>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="col-lg-12">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h3 class="panel-title"><i class="fa fa-area-chart"></i> System Performance Charts</h3>
                </div>
                <div class="panel-body">
                    <div class="row">
                        <div class="col-lg-6">
                            <div class="chart-container">
                                <h4>CPU Usage</h4>
                                <div id="cpu-chart" style="height: 300px;"></div>
                            </div>
                        </div>
                        <div class="col-lg-6">
                            <div class="chart-container">
                                <h4>Memory Usage</h4>
                                <div id="memory-chart" style="height: 300px;"></div>
                            </div>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-lg-6">
                            <div class="chart-container">
                                <h4>Overall Disk Usage</h4>
                                <div id="disk-chart" style="height: 300px;"></div>
                            </div>
                        </div>
                        <div class="col-lg-6">
                            <div class="chart-container">
                                <h4>Storage Paths Overview</h4>
                                <div id="storage-overview" style="height: 300px;"></div>
                            </div>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-lg-12">
                            <div class="chart-container">
                                <h4>Network Interface Traffic</h4>
                                <div id="network-chart" style="height: 300px;"></div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <div class="row">
        <div class="col-lg-12">
            <div class="panel panel-default">
                <div class="panel-heading">
                    <h3 class="panel-title"><i class="fa fa-hdd-o"></i> Storage Path Monitoring</h3>
                </div>
                <div class="panel-body">
                    <div id="storage-charts" class="row">
                        <!-- Storage charts will be dynamically generated here -->
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script>
    document.addEventListener('DOMContentLoaded', function() {
        let currentRange = '1h';
        let charts = {
            cpu: null,
            memory: null,
            disk: null,
            storage: null,
            storagePaths: {},
            network: null // Added for network traffic chart
        };
        
        // Initialize time range buttons
        document.querySelectorAll('.time-range-btn').forEach(function(btn) {
            btn.addEventListener('click', function() {
                document.querySelectorAll('.time-range-btn').forEach(b => {
                    b.classList.remove('btn-primary');
                    b.classList.add('btn-default');
                });
                this.classList.remove('btn-default');
                this.classList.add('btn-primary');
                currentRange = this.dataset.range;
                loadStats();
            });
        });
        
        // Set initial active button
        document.querySelector('.time-range-btn[data-range="1h"]').classList.add('btn-primary');
        document.querySelector('.time-range-btn[data-range="1h"]').classList.remove('btn-default');
        
        function loadStats() {
            console.log('Loading stats for range:', currentRange);
            
            // Show loading state
            document.getElementById('server-status').innerHTML = '<span class="text-info">Loading...</span>';
            
            // Create XMLHttpRequest
            const xhr = new XMLHttpRequest();
            xhr.open('GET', '/ajax/stats.php?range=' + currentRange, true);
            xhr.setRequestHeader('Content-Type', 'application/json');
            xhr.timeout = 15000; // 15 second timeout
            
            xhr.onload = function() {
                if (xhr.status === 200) {
                    try {
                        const data = JSON.parse(xhr.responseText);
                        console.log('Stats loaded successfully:', data);
                        
                        // Update all components
                        if (data.live) {
                            updateLiveStats(data.live);
                        }
                        if (data.historical) {
                            updatePerformanceCharts(data.historical);
                        }
                        if (data.storage) {
                            updateStorageCharts(data.storage);
                        }
                        if (data.network) {
                            updateNetworkInterfaces(data.network);
                        }
                        if (data.network_traffic) {
                            updateNetworkTrafficChart(data.network_traffic);
                        }
                        updateStatus(data);
                        
                        // Reset retry counter on success
                        retryCount = 0;
                        
                    } catch (e) {
                        console.error('Failed to parse JSON:', e);
                        console.error('Response text:', xhr.responseText);
                        document.getElementById('server-status').innerHTML = '<span class="text-danger">Error parsing data</span>';
                    }
                } else {
                    console.error('HTTP Error:', xhr.status);
                    console.error('Response text:', xhr.responseText);
                    document.getElementById('server-status').innerHTML = '<span class="text-danger">HTTP Error: ' + xhr.status + '</span>';
                }
            };
            
            xhr.onerror = function() {
                console.error('Network error occurred');
                document.getElementById('server-status').innerHTML = '<span class="text-danger">Network error</span>';
                
                // Retry after a short delay if this is the first attempt
                if (retryCount < maxRetries) {
                    retryCount++;
                    setTimeout(loadStats, 2000);
                }
            };
            
            xhr.ontimeout = function() {
                console.error('Request timed out');
                document.getElementById('server-status').innerHTML = '<span class="text-danger">Request timed out</span>';
                
                // Retry after a short delay if this is the first attempt
                if (retryCount < maxRetries) {
                    retryCount++;
                    setTimeout(loadStats, 2000);
                }
            };
            
            xhr.send();
        }
        
        function updateLiveStats(live) {
            if (!live) {
                document.getElementById('cpu-usage').textContent = '--';
                document.getElementById('memory-usage').textContent = '--';
                document.getElementById('disk-usage').textContent = '--';
                document.getElementById('server-status').innerHTML = '<span class="text-warning">No data available</span>';
                return;
            }
            
            document.getElementById('cpu-usage').textContent = live.cpu + '%';
            document.getElementById('memory-usage').textContent = live.memory + '%';
            document.getElementById('disk-usage').textContent = live.disk + '%';
            
            if (live.server_running) {
                document.getElementById('server-status').innerHTML = '<span class="text-success">Bluecherry Server is running</span>';
            } else {
                document.getElementById('server-status').innerHTML = '<span class="text-danger">Bluecherry Server is not running</span>';
            }
        }
        
        function updatePerformanceCharts(historical) {
            if (!historical || historical.length === 0) {
                return;
            }
            
            // Prepare data
            const labels = [];
            const cpuData = [];
            const memoryData = [];
            const diskData = [];
            
            historical.forEach(function(point) {
                const date = new Date(point.timestamp * 1000);
                labels.push(date.toLocaleTimeString());
                cpuData.push(point.cpu !== null ? point.cpu : null);
                memoryData.push(point.memory !== null ? point.memory : null);
                diskData.push(point.disk !== null ? point.disk : null);
            });
            
            // Create CPU Chart
            createChart('cpu-chart', 'CPU Usage (%)', cpuData, labels, 'rgb(255, 99, 132)', charts.cpu);
            
            // Create Memory Chart
            createChart('memory-chart', 'Memory Usage (%)', memoryData, labels, 'rgb(54, 162, 235)', charts.memory);
            
            // Create Disk Chart
            createChart('disk-chart', 'Disk Usage (%)', diskData, labels, 'rgb(255, 205, 86)', charts.disk);
        }
        
        function createChart(containerId, label, data, labels, color, existingChart) {
            const container = document.getElementById(containerId);
            if (!container) return;
            
            // Destroy existing chart
            if (existingChart) {
                existingChart.destroy();
            }
            
            // Check if Chart.js is available
            if (typeof Chart === 'undefined') {
                container.innerHTML = '<div class="text-center text-muted">Chart.js not available</div>';
                return;
            }
            
            // Create canvas
            const ctx = document.createElement('canvas');
            container.innerHTML = '';
            container.appendChild(ctx);
            
            // Create chart
            const chart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: label,
                        data: data,
                        borderColor: color,
                        backgroundColor: color.replace('rgb', 'rgba').replace(')', ', 0.1)'),
                        borderWidth: 2,
                        fill: false,
                        tension: 0.1
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            display: false
                        }
                    },
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 100,
                            title: {
                                display: true,
                                text: 'Usage (%)'
                            }
                        },
                        x: {
                            display: false
                        }
                    },
                    interaction: {
                        intersect: false,
                        mode: 'index'
                    }
                }
            });
            
            // Store chart reference
            if (containerId === 'cpu-chart') charts.cpu = chart;
            else if (containerId === 'memory-chart') charts.memory = chart;
            else if (containerId === 'disk-chart') charts.disk = chart;
            else if (containerId === 'network-chart') charts.network = chart; // Store network chart
        }
        
        function updateStorageCharts(storage) {
            if (!storage || storage.length === 0) {
                document.getElementById('storage-overview').innerHTML = '<div class="text-center text-muted">No storage data available</div>';
                return;
            }
            
            // Create storage overview chart
            createStorageOverviewChart(storage);
            
            // Create individual storage path charts
            createStoragePathCharts(storage);
        }
        
        function createStorageOverviewChart(storage) {
            const container = document.getElementById('storage-overview');
            if (!container) return;
            
            if (typeof Chart === 'undefined') {
                container.innerHTML = '<div class="text-center text-muted">Chart.js not available</div>';
                return;
            }
            
            // Destroy existing chart
            if (charts.storage) {
                charts.storage.destroy();
            }
            
            // Prepare data for pie chart - use actual API data
            const labels = storage.map(s => s.path);
            const data = storage.map(s => s.usage_percent);
            const colors = [
                'rgb(255, 99, 132)', 'rgb(54, 162, 235)', 'rgb(255, 205, 86)',
                'rgb(75, 192, 192)', 'rgb(153, 102, 255)', 'rgb(255, 159, 64)',
                'rgb(199, 199, 199)', 'rgb(83, 102, 255)'
            ];
            
            // Create canvas
            const ctx = document.createElement('canvas');
            container.innerHTML = '';
            container.appendChild(ctx);
            
            // Create chart
            charts.storage = new Chart(ctx, {
                type: 'doughnut',
                data: {
                    labels: labels,
                    datasets: [{
                        data: data,
                        backgroundColor: colors.slice(0, data.length),
                        borderWidth: 2
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            position: 'bottom',
                            labels: {
                                boxWidth: 12
                            }
                        }
                    }
                }
            });
        }
        
        function createStoragePathCharts(storage) {
            const container = document.getElementById('storage-charts');
            if (!container) return;
            
            // Clear existing charts
            container.innerHTML = '';
            
            // Create a chart for each storage path from API data
            storage.forEach(function(pathData, index) {
                const colDiv = document.createElement('div');
                colDiv.className = 'col-lg-6 col-md-6';
                colDiv.style.marginBottom = '20px';
                
                const chartId = 'storage-chart-' + index;
                const totalGB = (pathData.total_size / (1024 * 1024 * 1024)).toFixed(2);
                const usedGB = (pathData.used_size / (1024 * 1024 * 1024)).toFixed(2);
                const freeGB = (pathData.free_size / (1024 * 1024 * 1024)).toFixed(2);
                
                colDiv.innerHTML = `
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <h4 class="panel-title">${pathData.path} (${pathData.filesystem})</h4>
                        </div>
                        <div class="panel-body">
                            <div class="row">
                                <div class="col-xs-6">
                                    <div class="text-center">
                                        <div class="huge" style="color: ${getUsageColor(pathData.usage_percent)}">${pathData.usage_percent}%</div>
                                        <div>Usage</div>
                                    </div>
                                </div>
                                <div class="col-xs-6">
                                    <div class="text-center">
                                        <div class="huge">${usedGB} GB</div>
                                        <div>Used / ${totalGB} GB</div>
                                    </div>
                                </div>
                            </div>
                            <div id="${chartId}" style="height: 200px; margin-top: 15px;"></div>
                        </div>
                    </div>
                `;
                
                container.appendChild(colDiv);
                
                // Create mini chart for this storage path
                createStoragePathMiniChart(chartId, {
                    used_gb: parseFloat(usedGB),
                    free_gb: parseFloat(freeGB),
                    usage_percent: pathData.usage_percent
                });
            });
        }
        
        function createStoragePathMiniChart(containerId, pathData) {
            const container = document.getElementById(containerId);
            if (!container || typeof Chart === 'undefined') return;
            
            // Create a simple bar chart showing usage
            const ctx = document.createElement('canvas');
            container.appendChild(ctx);
            
            const chart = new Chart(ctx, {
                type: 'bar',
                data: {
                    labels: ['Used', 'Free'],
                    datasets: [{
                        data: [pathData.used_gb, pathData.free_gb],
                        backgroundColor: [
                            getUsageColor(pathData.usage_percent),
                            'rgb(200, 200, 200)'
                        ],
                        borderWidth: 0
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            display: false
                        }
                    },
                    scales: {
                        y: {
                            beginAtZero: true,
                            display: false
                        },
                        x: {
                            display: false
                        }
                    }
                }
            });
            
            // Store chart reference
            charts.storagePaths[containerId] = chart;
        }
        
        function getUsageColor(usage) {
            if (usage >= 90) return 'rgb(220, 53, 69)'; // Red
            if (usage >= 75) return 'rgb(255, 193, 7)'; // Yellow
            return 'rgb(40, 167, 69)'; // Green
        }

        function updateNetworkInterfaces(network) {
            const container = document.getElementById('network-interfaces');
            if (!container) return;

            // Clear existing interface panels
            container.innerHTML = '';

            if (!network || network.length === 0) {
                container.innerHTML = '<div class="col-lg-12 text-center text-muted">No network interface data available.</div>';
                return;
            }

            network.forEach(function(ifaceData) {
                const colDiv = document.createElement('div');
                colDiv.className = 'col-lg-4 col-md-6'; // Adjusted for 3 columns
                colDiv.style.marginBottom = '20px';

                const statusColor = ifaceData.is_up ? 'text-success' : 'text-danger';
                const statusIcon = ifaceData.is_up ? 'fa-check-circle' : 'fa-times-circle';
                const statusText = ifaceData.is_up ? 'Up' : 'Down';

                colDiv.innerHTML = `
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <h4 class="panel-title">${ifaceData.name}</h4>
                        </div>
                        <div class="panel-body">
                            <div class="row">
                                <div class="col-xs-6">
                                    <div class="text-center">
                                        <i class="fa ${statusIcon} fa-3x ${statusColor}"></i>
                                    </div>
                                </div>
                                <div class="col-xs-6">
                                    <div class="text-center">
                                        <div class="huge">${statusText}</div>
                                        <div>Status</div>
                                    </div>
                                </div>
                            </div>
                            <div class="row" style="margin-top: 10px;">
                                <div class="col-xs-12">
                                    <small class="text-muted">
                                        <strong>RX:</strong> ${(ifaceData.rx_bytes / (1024 * 1024)).toFixed(2)} MB<br>
                                        <strong>TX:</strong> ${(ifaceData.tx_bytes / (1024 * 1024)).toFixed(2)} MB
                                    </small>
                                </div>
                            </div>
                        </div>
                    </div>
                `;
                container.appendChild(colDiv);
            });
        }

        function updateNetworkTrafficChart(networkTraffic) {
            const container = document.getElementById('network-chart');
            if (!container || typeof Chart === 'undefined') return;

            // Destroy existing chart
            if (charts.network) {
                charts.network.destroy();
            }

            if (!networkTraffic || networkTraffic.length === 0) {
                container.innerHTML = '<div class="text-center text-muted">No network traffic data available.</div>';
                return;
            }

            const labels = networkTraffic.map(t => new Date(t.timestamp * 1000).toLocaleTimeString());
            const uploadData = networkTraffic.map(t => t.upload_bytes / 1024 / 1024); // MB
            const downloadData = networkTraffic.map(t => t.download_bytes / 1024 / 1024); // MB

            const ctx = document.createElement('canvas');
            container.innerHTML = '';
            container.appendChild(ctx);

            charts.network = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [
                        {
                            label: 'Upload (MB)',
                            data: uploadData,
                            borderColor: 'rgb(54, 162, 235)',
                            backgroundColor: 'rgba(54, 162, 235, 0.1)',
                            borderWidth: 2,
                            fill: false,
                            tension: 0.1
                        },
                        {
                            label: 'Download (MB)',
                            data: downloadData,
                            borderColor: 'rgb(255, 99, 132)',
                            backgroundColor: 'rgba(255, 99, 132, 0.1)',
                            borderWidth: 2,
                            fill: false,
                            tension: 0.1
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: {
                            position: 'bottom',
                            labels: {
                                boxWidth: 12
                            }
                        }
                    },
                    scales: {
                        y: {
                            beginAtZero: true,
                            title: {
                                display: true,
                                text: 'Data (MB)'
                            }
                        },
                        x: {
                            display: false
                        }
                    },
                    interaction: {
                        intersect: false,
                        mode: 'index'
                    }
                }
            });
        }
        
        function updateStatus(data) {
            const statusElement = document.getElementById('rrd-status');
            if (data.rrd_available) {
                statusElement.innerHTML = '<span class="text-success"><i class="fa fa-check"></i> Historical data available</span>';
            } else {
                statusElement.innerHTML = '<span class="text-warning"><i class="fa fa-exclamation-triangle"></i> Live data only</span>';
            }
        }
        
        // Load initial data with retry mechanism
        let retryCount = 0;
        const maxRetries = 3;
        
        function loadStatsWithRetry() {
            loadStats();
        }
        
        function toggleDebug() {
            const debugPanel = document.getElementById('debug-panel');
            const debugInfo = document.getElementById('debug-info');
            
            if (debugPanel.style.display === 'none') {
                debugPanel.style.display = 'block';
                
                // Collect debug information
                const debugData = {
                    'Current Time': new Date().toLocaleString(),
                    'Current Range': currentRange,
                    'Retry Count': retryCount,
                    'Chart.js Available': typeof Chart !== 'undefined',
                    'User Agent': navigator.userAgent,
                    'Page URL': window.location.href,
                    'Last API Call': new Date().toLocaleString()
                };
                
                let debugHtml = '<table class="table table-striped">';
                for (const [key, value] of Object.entries(debugData)) {
                    debugHtml += `<tr><td><strong>${key}:</strong></td><td>${value}</td></tr>`;
                }
                debugHtml += '</table>';
                
                debugInfo.innerHTML = debugHtml;
            } else {
                debugPanel.style.display = 'none';
            }
        }
        
        // Load initial data
        loadStatsWithRetry();
        
        // Refresh every 30 seconds
        setInterval(loadStatsWithRetry, 30000);
    });
    </script>
</body>
</html> 