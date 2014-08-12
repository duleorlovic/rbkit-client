// Generated by CoffeeScript 1.7.1
var chart,
  __bind = function(fn, me){ return function(){ return fn.apply(me, arguments); }; };

this.Chart = (function() {
  var dataForNewSeries, placeHolderSeries;

  placeHolderSeries = function() {
    var num, _i, _results;
    _results = [];
    for (num = _i = -9; _i <= 0; num = ++_i) {
      _results.push([(new Date()).getTime() + num * 1000, 0]);
    }
    return _results;
  };

  dataForNewSeries = function(series, currentTime, count) {
    var data, newData, num;
    data = series.data;
    newData = (function() {
      var _i, _results;
      _results = [];
      for (num = _i = 0; _i <= 8; num = ++_i) {
        _results.push([data[num].x, 0]);
      }
      return _results;
    })();
    newData.push([currentTime, count]);
    return newData;
  };

  function Chart() {
    this.updateChart = __bind(this.updateChart, this);
    this.updateGcStats = __bind(this.updateGcStats, this);
    this.receiveLiveData = __bind(this.receiveLiveData, this);
    this.establishQtBridge = __bind(this.establishQtBridge, this);
    this.tryQtBridge = __bind(this.tryQtBridge, this);
    this.objectCounter = new ObjectCount();
    this.knownClasses = {
      "String": true
    };
    this.gcStat = new GcStat();
    this.legendIndex = 1;
  }

  Chart.prototype.plotChart = function() {
    this.chart = $("#object-container").highcharts({
      chart: {
        type: 'column'
      },
      title: {
        text: null
      },
      xAxis: {
        type: 'datetime',
        tickPixelInterval: 150,
        title: {
          enabled: false
        }
      },
      yAxis: {
        min: 0,
        title: {
          text: 'Live Objects'
        },
        stackLabels: {
          enabled: true,
          style: {
            fontWeight: 'bold',
            color: (Highcharts.theme && Highcharts.theme.textColor) || 'gray'
          }
        }
      },
      plotOptions: {
        column: {
          stacking: 'normal',
          dataLabels: {
            enabled: false,
            color: (Highcharts.theme && Highcharts.theme.dataLabelsColor) || 'white',
            style: {
              textShadow: '0 0 3px black, 0 0 3px black'
            }
          }
        }
      },
      series: [
        {
          name: 'String',
          data: placeHolderSeries()
        }
      ]
    }).highcharts();
    return this.gcStat.plotChart();
  };

  Chart.prototype.tryQtBridge = function() {
    return window.setTimeout(this.establishQtBridge, 1000);
  };

  Chart.prototype.establishQtBridge = function() {
    var _ref;
    setInterval(this.updateChart, 1500);
    return (_ref = window.jsBridge) != null ? _ref.jsEvent.connect(this.receiveLiveData) : void 0;
  };

  Chart.prototype.receiveLiveData = function(data) {
    switch (data.event_type) {
      case "object_stats":
        return this.addToCurrentObjects(data.payload);
      case "gc_start":
        return this.gcStat.gcStarted(data);
      case "gc_stop":
        return this.gcStat.gcStopped(data);
      case "gc_stats":
        return this.updateGcStats(data.payload);
    }
  };

  Chart.prototype.updateGcStats = function(gcStats) {
    var $stats, importantFields, key, row, value, _i, _len, _results;
    $stats = $('#gc-stats-table tbody');
    $stats.empty();
    importantFields = ['count', 'minor_gc_count', 'major_gc_count', 'heap_length', 'heap_eden_page_length', 'heap_used', 'heap_live_slot', 'heap_free_slot', 'heap_swept_slot', 'old_object', 'old_object_limit', 'remembered_shady_object', 'total_allocated_object', 'total_freed_object'];
    _results = [];
    for (_i = 0, _len = importantFields.length; _i < _len; _i++) {
      key = importantFields[_i];
      value = gcStats[key];
      row = "<tr><td>" + key + "</td><td>" + value + "</td></tr>";
      _results.push($stats.append(row));
    }
    return _results;
  };

  Chart.prototype.addToCurrentObjects = function(liveObjectCount) {
    return this.objectCounter.addToCurrentObjects(liveObjectCount);
  };

  Chart.prototype.updateChart = function() {
    var count, currentTime, objectType, timeSeries;
    currentTime = (new Date()).getTime();
    timeSeries = this.objectCounter.timeSeries();
    for (objectType in timeSeries) {
      count = timeSeries[objectType];
      if (this.knownClasses[objectType] != null) {
        this.addNewDataPoint(objectType, count, currentTime);
      } else {
        this.addNewSeries(objectType, count, currentTime);
      }
    }
    return this.gcStat.updateChart();
  };

  Chart.prototype.addNewDataPoint = function(objectType, count, currentTime) {
    var selectedSeries, series;
    selectedSeries = ((function() {
      var _i, _len, _ref, _results;
      _ref = this.chart.series;
      _results = [];
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        series = _ref[_i];
        if (series.name === objectType) {
          _results.push(series);
        }
      }
      return _results;
    }).call(this))[0];
    if (selectedSeries != null) {
      return selectedSeries.addPoint([currentTime, count], true, true);
    }
  };

  Chart.prototype.addNewSeries = function(objectType, count, currentTime) {
    this.chart.addSeries({
      name: objectType,
      data: dataForNewSeries(this.chart.series[0], currentTime, count),
      legendIndex: this.legendIndex
    }, true, true);
    this.knownClasses[objectType] = true;
    return this.legendIndex += 1;
  };

  return Chart;

})();

chart = new Chart();

chart.plotChart();

chart.tryQtBridge();
