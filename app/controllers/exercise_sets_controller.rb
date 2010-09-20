class ExerciseSetsController < ApplicationController
  # GET /exercise_sets
  # GET /exercise_sets.xml
  def index
    @exercise_sets = ExerciseSet.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @exercise_sets }
    end
  end

  # GET /exercise_sets/1
  # GET /exercise_sets/1.xml
  def show
    @exercise_set = ExerciseSet.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @exercise_set }
    end
  end

  # GET /exercise_sets/new
  # GET /exercise_sets/new.xml
  def new
    @exercise_set = ExerciseSet.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @exercise_set }
    end
  end

  # GET /exercise_sets/1/edit
  def edit
    @exercise_set = ExerciseSet.find(params[:id])
  end

  # POST /exercise_sets
  # POST /exercise_sets.xml
  def create
    @exercise_set = ExerciseSet.new(params[:exercise_set])

    respond_to do |format|
      if @exercise_set.save
        format.html { redirect_to(@exercise_set, :notice => 'Exercise set was successfully created.') }
        format.xml  { render :xml => @exercise_set, :status => :created, :location => @exercise_set }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @exercise_set.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /exercise_sets/1
  # PUT /exercise_sets/1.xml
  def update
    @exercise_set = ExerciseSet.find(params[:id])

    respond_to do |format|
      if @exercise_set.update_attributes(params[:exercise_set])
        format.html { redirect_to(@exercise_set, :notice => 'Exercise set was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @exercise_set.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /exercise_sets/1
  # DELETE /exercise_sets/1.xml
  def destroy
    @exercise_set = ExerciseSet.find(params[:id])
    @exercise_set.destroy

    respond_to do |format|
      format.html { redirect_to(exercise_sets_url) }
      format.xml  { head :ok }
    end
  end
end
