// Set up the temperature chart
const tempCtx = document.getElementById('temperatureChart').getContext('2d');
const tempChart = new Chart(tempCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Temperature (°C)',
            data: [],
            borderColor: 'rgba(255, 99, 132, 1)',
            backgroundColor: 'rgba(255, 99, 132, 0.2)',
            borderWidth: 2,
            fill: true
        }]
    },
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: 'Time' } },
            y: { title: { display: true, text: 'Temperature (°C)' } }
        }
    }
});

// Set up the humidity chart
const humidityCtx = document.getElementById('humidityChart').getContext('2d');
const humidityChart = new Chart(humidityCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Humidity (%)',
            data: [],
            borderColor: 'rgba(75, 192, 192, 1)',
            backgroundColor: 'rgba(75, 192, 192, 0.2)',
            borderWidth: 2,
            fill: true
        }]
    },
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: 'Time' } },
            y: { title: { display: true, text: 'Humidity (%)' } }
        }
    }
});

// Set up the air quality chart (replacing the distance chart)
const airQualityCtx = document.getElementById('airQualityChart').getContext('2d');
const airQualityChart = new Chart(airQualityCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Air Quality (PPM)',
            data: [],
            borderColor: 'rgba(153, 102, 255, 1)',
            backgroundColor: 'rgba(153, 102, 255, 0.2)',
            borderWidth: 2,
            fill: true
        }]
    },
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: 'Time' } },
            y: { title: { display: true, text: 'Air Quality (PPM)' } }
        }
    }
});

// Set up the flame chart
const flameCtx = document.getElementById('flameChart').getContext('2d');
const flameChart = new Chart(flameCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Flame Sensor',
            data: [],
            borderColor: 'rgba(255, 159, 64, 1)',
            backgroundColor: 'rgba(255, 159, 64, 0.2)',
            borderWidth: 2,
            fill: true
        }]
    },
    options: {
        responsive: true,
        scales: {
            x: { title: { display: true, text: 'Time' } },
            y: { 
                title: { display: true, text: 'Flame Sensor' },
                min: 0,
                max: 4095
            }
        }
    }
});

// Function to fetch data from the ESP32 server
async function fetchSensorData() {
    try {
        const response = await fetch('http://192.168.1.7/data'); // Replace with your ESP32 IP
        const data = await response.json();

        const now = new Date().toLocaleTimeString();

        // Update Temperature Chart
        tempChart.data.labels.push(now);
        tempChart.data.datasets[0].data.push(data.temperature);
        tempChart.update();

        // Update Humidity Chart
        humidityChart.data.labels.push(now);
        humidityChart.data.datasets[0].data.push(data.humidity);
        humidityChart.update();

        // Update Air Quality Chart (MQ135 sensor)
        airQualityChart.data.labels.push(now);
        airQualityChart.data.datasets[0].data.push(data.airQuality);
        airQualityChart.update();

        // Update Flame Sensor Chart (0 for no flame, 4095 for flame detected)
        flameChart.data.labels.push(now);
        flameChart.data.datasets[0].data.push(data.flameDetected === 'Yes' ? 4095 : 0);
        flameChart.update();

        // Update Stats
        document.getElementById('avg-temp').textContent = `${data.temperature} °C`;
        document.getElementById('avg-humidity').textContent = `${data.humidity} %`;
        document.getElementById('avg-airquality').textContent = `${data.airQuality} PPM`; // Show Air Quality PPM
        document.getElementById('avg-flame').textContent = data.flameDetected ? "Yes" : "No";

        // Check for Thresholds
        const thresholdWarning = document.getElementById('warning');
        if (data.thresholdExceeded) {
            thresholdWarning.style.display = 'block';
            thresholdWarning.textContent = `Warning: ${data.exceededParameter} Exceeded!`;
        } else {
            thresholdWarning.style.display = 'none';
        }

    } catch (error) {
        console.error("Error fetching data from ESP:", error);
    }
}

// Set an interval to fetch data every 2 seconds
setInterval(fetchSensorData, 2000);
