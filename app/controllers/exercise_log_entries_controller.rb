class ExerciseLogEntriesController < ApplicationController
  # GET /exercise_log_entries
  # GET /exercise_log_entries.xml
  def index
    @exercise_log_entries = ExerciseLogEntry.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @exercise_log_entries }
    end
  end

  # GET /exercise_log_entries/1
  # GET /exercise_log_entries/1.xml
  def show
    @exercise_log_entry = ExerciseLogEntry.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @exercise_log_entry }
    end
  end

  # GET /exercise_log_entries/new
  # GET /exercise_log_entries/new.xml
  def new
    @exercise_log_entry = ExerciseLogEntry.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @exercise_log_entry }
    end
  end

  # GET /exercise_log_entries/1/edit
  def edit
    @exercise_log_entry = ExerciseLogEntry.find(params[:id])
  end

  # POST /exercise_log_entries
  # POST /exercise_log_entries.xml
  def create
    @exercise_log_entry = ExerciseLogEntry.new(params[:exercise_log_entry])

    respond_to do |format|
      if @exercise_log_entry.save
        format.html { redirect_to(@exercise_log_entry, :notice => 'Exercise log entry was successfully created.') }
        format.xml  { render :xml => @exercise_log_entry, :status => :created, :location => @exercise_log_entry }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @exercise_log_entry.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /exercise_log_entries/1
  # PUT /exercise_log_entries/1.xml
  def update
    @exercise_log_entry = ExerciseLogEntry.find(params[:id])

    respond_to do |format|
      if @exercise_log_entry.update_attributes(params[:exercise_log_entry])
        format.html { redirect_to(@exercise_log_entry, :notice => 'Exercise log entry was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @exercise_log_entry.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /exercise_log_entries/1
  # DELETE /exercise_log_entries/1.xml
  def destroy
    @exercise_log_entry = ExerciseLogEntry.find(params[:id])
    @exercise_log_entry.destroy

    respond_to do |format|
      format.html { redirect_to(exercise_log_entries_url) }
      format.xml  { head :ok }
    end
  end
end
