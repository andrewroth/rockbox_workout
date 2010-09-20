class ExerciseTypesController < ApplicationController
  # GET /exercise_types
  # GET /exercise_types.xml
  def index
    @exercise_types = ExerciseType.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @exercise_types }
    end
  end

  # GET /exercise_types/1
  # GET /exercise_types/1.xml
  def show
    @exercise_type = ExerciseType.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @exercise_type }
    end
  end

  # GET /exercise_types/new
  # GET /exercise_types/new.xml
  def new
    @exercise_type = ExerciseType.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @exercise_type }
    end
  end

  # GET /exercise_types/1/edit
  def edit
    @exercise_type = ExerciseType.find(params[:id])
  end

  # POST /exercise_types
  # POST /exercise_types.xml
  def create
    @exercise_type = ExerciseType.new(params[:exercise_type])

    respond_to do |format|
      if @exercise_type.save
        format.html { redirect_to(@exercise_type, :notice => 'Exercise type was successfully created.') }
        format.xml  { render :xml => @exercise_type, :status => :created, :location => @exercise_type }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @exercise_type.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /exercise_types/1
  # PUT /exercise_types/1.xml
  def update
    @exercise_type = ExerciseType.find(params[:id])

    respond_to do |format|
      if @exercise_type.update_attributes(params[:exercise_type])
        format.html { redirect_to(@exercise_type, :notice => 'Exercise type was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @exercise_type.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /exercise_types/1
  # DELETE /exercise_types/1.xml
  def destroy
    @exercise_type = ExerciseType.find(params[:id])
    @exercise_type.destroy

    respond_to do |format|
      format.html { redirect_to(exercise_types_url) }
      format.xml  { head :ok }
    end
  end
end
