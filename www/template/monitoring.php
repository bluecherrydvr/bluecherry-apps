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
    <h1>System Monitoring</h1>
    <div id="charts">
        <div class="chart-container">
            <h2>CPU Usage</h2>
            <canvas id="cpuChart"></canvas>
        </div>
        <div class="chart-container">
            <h2>Memory Usage</h2>
            <canvas id="memoryChart"></canvas>
        </div>
        <!-- Add more charts as needed -->
    </div>
    <script>
    function fetchStatsAndRender() {
        fetch('/ajax/monitoring/stats.php')
            .then(res => {
                if (!res.ok) throw new Error('Not logged in or API error');
                return res.json();
            })
            .then(data => {
                // Example assumes API returns { timestamps: [...], cpu: [...], memory: [...] }
                renderChart('cpuChart', 'CPU Usage', data.timestamps, data.cpu);
                renderChart('memoryChart', 'Memory Usage', data.timestamps, data.memory);
            })
            .catch(err => {
                alert('You must be logged in to view stats.');
            });
    }

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
                        fill: false
                    }]
                },
                options: {
                    responsive: true,
                    scales: { x: { display: true }, y: { display: true } }
                }
            });
        }
    }

    setInterval(fetchStatsAndRender, 10000); // Refresh every 10 seconds
    fetchStatsAndRender();
    </script>
</body>
</html> 