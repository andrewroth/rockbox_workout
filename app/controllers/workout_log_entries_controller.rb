class WorkoutLogEntriesController < ApplicationController
  # GET /workout_log_entries
  # GET /workout_log_entries.xml
  def index
    @workout_log_entries = WorkoutLogEntry.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @workout_log_entries }
    end
  end

  # GET /workout_log_entries/1
  # GET /workout_log_entries/1.xml
  def show
    @workout_log_entry = WorkoutLogEntry.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @workout_log_entry }
    end
  end

  # GET /workout_log_entries/new
  # GET /workout_log_entries/new.xml
  def new
    @workout_log_entry = WorkoutLogEntry.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @workout_log_entry }
    end
  end

  # GET /workout_log_entries/1/edit
  def edit
    @workout_log_entry = WorkoutLogEntry.find(params[:id])
  end

  # POST /workout_log_entries
  # POST /workout_log_entries.xml
  def create
    @workout_log_entry = WorkoutLogEntry.new(params[:workout_log_entry])

    respond_to do |format|
      if @workout_log_entry.save
        format.html { redirect_to(@workout_log_entry, :notice => 'Workout log entry was successfully created.') }
        format.xml  { render :xml => @workout_log_entry, :status => :created, :location => @workout_log_entry }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @workout_log_entry.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /workout_log_entries/1
  # PUT /workout_log_entries/1.xml
  def update
    @workout_log_entry = WorkoutLogEntry.find(params[:id])

    respond_to do |format|
      if @workout_log_entry.update_attributes(params[:workout_log_entry])
        format.html { redirect_to(@workout_log_entry, :notice => 'Workout log entry was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @workout_log_entry.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /workout_log_entries/1
  # DELETE /workout_log_entries/1.xml
  def destroy
    @workout_log_entry = WorkoutLogEntry.find(params[:id])
    @workout_log_entry.destroy

    respond_to do |format|
      format.html { redirect_to(workout_log_entries_url) }
      format.xml  { head :ok }
    end
  end
end
