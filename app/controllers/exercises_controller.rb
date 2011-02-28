class ExercisesController < ApplicationController

  def make_standard_sets
    @exercise = Exercise.find(params[:id])
    es = @exercise.exercise_sets.find_or_create_by_name "1st"
    es.position = 1
    es.save!
    es = @exercise.exercise_sets.find_or_create_by_name "2nd"
    es.position = 2
    es.save!
    es = @exercise.exercise_sets.find_or_create_by_name "3rd"
    es.position = 3
    es.save!
    flash[:notice] = "Standard sets 1st, 2nd, 3rd made for #{@exercise.name}"
    redirect_to exercises_url
  end

  # GET /exercises
  # GET /exercises.xml
  def index
    @exercises = Exercise.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @exercises }
    end
  end

  # GET /exercises/1
  # GET /exercises/1.xml
  def show
    @exercise = Exercise.find(params[:id])

    respond_to do |format|
      format.html {
        @graph = open_flash_chart_object(600, 300, url_for(:action => 'show', :format => :json))
      }

      format.json { 
        chart = setup_chart @exercise
        puts chart.to_s
        render :text => chart, :layout => false
      }                   

      format.xml  { render :xml => @exercise }
    end
  end

  # GET /exercises/new
  # GET /exercises/new.xml
  def new
    @exercise = Exercise.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @exercise }
    end
  end

  # GET /exercises/1/edit
  def edit
    @exercise = Exercise.find(params[:id])
  end

  # POST /exercises
  # POST /exercises.xml
  def create
    @exercise = Exercise.new(params[:exercise])

    respond_to do |format|
      if @exercise.save
        format.html { redirect_to(@exercise, :notice => 'Exercise was successfully created.') }
        format.xml  { render :xml => @exercise, :status => :created, :location => @exercise }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @exercise.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /exercises/1
  # PUT /exercises/1.xml
  def update
    @exercise = Exercise.find(params[:id])

    respond_to do |format|
      if @exercise.update_attributes(params[:exercise])
        format.html { redirect_to(@exercise, :notice => 'Exercise was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @exercise.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /exercises/1
  # DELETE /exercises/1.xml
  def destroy
    @exercise = Exercise.find(params[:id])
    @exercise.destroy

    respond_to do |format|
      format.html { redirect_to(exercises_url) }
      format.xml  { head :ok }
    end
  end

  protected

    def setup_chart(exercise)
      title = Title.new(exercise.name)

      data1 = []
      data2 = []
      data3 = []

      10.times do |x|
        data1 << rand(5) + 1
        data2 << rand(6) + 7
        data3 << rand(5) + 14
      end

      max_y = 0
      lines = []
      for ex_set in exercise.exercise_sets
        # weight
        line = Line.new
        line.text = "Weight-#{ex_set.name}"
        line.width = 1
        line.colour = '#5E4725'
        line.dot_size = 5
        line.values = ex_set.weights_goal_arr
        max_y = (m = ex_set.weights_goal_arr.collect(&:to_i).max) > max_y ? m : max_y
        lines << line

        # amount
        line = Line.new
        line.text = "Reps-#{ex_set.name}"
        line.width = 2
        line.colour = '#6363AC'
        line.dot_size = 10
        line.values = ex_set.reps_goal_arr
        max_y = (m = ex_set.reps_goal_arr.collect(&:to_i).max) > max_y ? m : max_y

        lines << line
      end

      y = YAxis.new
      y.set_range(0,max_y*1.2,5)

      x_legend = XLegend.new("n")
      x_legend.set_style('{font-size: 20px; color: #778877}')

      y_legend = YLegend.new("")
      y_legend.set_style('{font-size: 20px; color: #770077}')

      chart = OpenFlashChart.new
      chart.set_title(title)
      chart.set_x_legend(x_legend)
      chart.set_y_legend(y_legend)
      chart.y_axis = y

      for line in lines
        chart.add_element(line)
      end

      return chart
    end
end
