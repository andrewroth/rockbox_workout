class WorkoutExercisesController < ApplicationController
  # GET /workout_exercises
  # GET /workout_exercises.xml
  def index
    @workout_exercises = WorkoutExercise.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @workout_exercises }
    end
  end

  # GET /workout_exercises/1
  # GET /workout_exercises/1.xml
  def show
    @workout_exercise = WorkoutExercise.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @workout_exercise }
    end
  end

  # GET /workout_exercises/new
  # GET /workout_exercises/new.xml
  def new
    @workout_exercise = WorkoutExercise.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @workout_exercise }
    end
  end

  # GET /workout_exercises/1/edit
  def edit
    @workout_exercise = WorkoutExercise.find(params[:id])
  end

  # POST /workout_exercises
  # POST /workout_exercises.xml
  def create
    @workout_exercise = WorkoutExercise.new(params[:workout_exercise])

    respond_to do |format|
      if @workout_exercise.save
        format.html { redirect_to(@workout_exercise, :notice => 'Workout exercise was successfully created.') }
        format.xml  { render :xml => @workout_exercise, :status => :created, :location => @workout_exercise }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @workout_exercise.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /workout_exercises/1
  # PUT /workout_exercises/1.xml
  def update
    @workout_exercise = WorkoutExercise.find(params[:id])

    respond_to do |format|
      if @workout_exercise.update_attributes(params[:workout_exercise])
        format.html { redirect_to(@workout_exercise, :notice => 'Workout exercise was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @workout_exercise.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /workout_exercises/1
  # DELETE /workout_exercises/1.xml
  def destroy
    @workout_exercise = WorkoutExercise.find(params[:id])
    @workout_exercise.destroy

    respond_to do |format|
      format.html { redirect_to(workout_exercises_url) }
      format.xml  { head :ok }
    end
  end
end
