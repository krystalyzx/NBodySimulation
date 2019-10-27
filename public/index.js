
function getRandomColor() {
    var letters = '0123456789ABCDEF';
    var color = '#';
    for (var i = 0; i < 6; i++) {
      color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
  }
  
  

var app = new Vue({
    el: "#app",
    data: {
      chart: null,
      loading: false,
      bodyvalue: [],
      i: 0,
      interval: null
    },
    created: function(){
        this.loading = true;
        
        if (this.chart != null) {
          this.chart.destroy();
        }
        
  
        axios
          .get("http://localhost:3000/data")
          .then(response => {
             
            values = response.data.split("\n");
            nbody = parseInt(values[0]);
            this.bodyvalue = [];
            
            for(i = 1; i < values.length - 1; i = i + nbody){
                roundvalue = [];
                for(j = i; j < i + nbody; j++){
                    splitedVal = values[j].split(" ");
                    roundvalue.push({x: parseFloat(splitedVal[0]),y:parseFloat(splitedVal[1]) });
                }
                this.bodyvalue.push(roundvalue);
            }
            console.log(this.bodyvalue);
            xmax = Math.max.apply(Math, this.bodyvalue[0].map(function(o){return o.x}));
            xmin = Math.min.apply(Math, this.bodyvalue[0].map(function(o){return o.x}));
            ymax = Math.max.apply(Math, this.bodyvalue[0].map(function(o){return o.y}));
            ymin = Math.min.apply(Math, this.bodyvalue[0].map(function(o){return o.y}));
            
            var pointBackgroundColors = [];
            for(i = 0; i < nbody; i++){
                pointBackgroundColors.push(getRandomColor());
            }
            var ctx = document.getElementById("myChart");

            this.chart = new Chart(ctx, {
                type: 'scatter',
                data: {
                    datasets: [{
                        label: 'Scatter Dataset',
                        data: this.bodyvalue[0],
                        pointBackgroundColor: pointBackgroundColors
                    }]
                },
             
                options: {
                    scales: {
                        xAxes: [{
                            type: 'linear',
                            position: 'bottom',
                            ticks : {
                              max : xmax,    
                              min : xmin
                          }
                        }],
                        yAxes: [{
                          type: 'linear',
                          position: 'bottom',
                          ticks : {
                            max : ymax,    
                            min : ymin
                        }
                      }]
                    }
                }
            });
      

            var self = this;
            this. interval = setInterval(function(){
                self.i = self.i + 1;
                if(self.i == self.bodyvalue.length){
                    clearInterval(self.interval);
                }else{
                    self.chart.data.datasets[0].data = self.bodyvalue[self.i];
                    self.chart.update();

                }
            },1);

          })
          .catch(error => {
            console.log(error);
            this.errored = true;
          })
          .finally(() => (this.loading = false));
    },
    methods: {
      getData: function() {
        console.log("hihi");
      
      }
    }
  });