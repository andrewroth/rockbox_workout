class WorkoutDatesController < ApplicationController
  # GET /workout_dates
  # GET /workout_dates.xml
  def index
    @workout_dates = WorkoutDate.all

    if params[:show_finished] == 'true'
      @workout_dates = WorkoutDate.all
    else
      @workout_dates = WorkoutDate.find_all_by_finished_at nil
    end

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @workout_dates }
    end
  end

  # GET /workout_dates/1
  # GET /workout_dates/1.xml
  def show
    @workout_date = WorkoutDate.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @workout_date }
    end
  end

  # GET /workout_dates/new
  # GET /workout_dates/new.xml
  def new
    @workout_date = WorkoutDate.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @workout_date }
    end
  end

  # GET /workout_dates/1/edit
  def edit
    @workout_date = WorkoutDate.find(params[:id])
  end

  # POST /workout_dates
  # POST /workout_dates.xml
  def create
    @workout_date = WorkoutDate.new(params[:workout_date])
    @workout_date.n ||= WorkoutDate.next_free_n(@workout_date.workout_id)

    respond_to do |format|
      if @workout_date.save
        format.html { redirect_to(@workout_date, :notice => 'Workout date was successfully created.') }
        format.xml  { render :xml => @workout_date, :status => :created, :location => @workout_date }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @workout_date.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /workout_dates/1
  # PUT /workout_dates/1.xml
  def update
    @workout_date = WorkoutDate.find(params[:id])
    @workout_date.n ||= WorkoutDate.next_free_n(@workout_date.workout_id)

    respond_to do |format|
      @workout_date.attributes = params[:workout_date]
      @workout_date.n ||= WorkoutDate.next_free_n(@workout_date.workout_id)

      if @workout_date.save
        format.html { redirect_to(@workout_date, :notice => 'Workout date was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @workout_date.errors, :status => :unprocessable_entity }
      end
    end
  end

  def finish
    @workout_date = WorkoutDate.find(params[:id])
    @workout_date.finished_at = Time.now
    @workout_date.save!
    flash[:notify] = "Workout date finished."
    redirect_to workout_dates_path
  end

  # DELETE /workout_dates/1
  # DELETE /workout_dates/1.xml
  def destroy
    @workout_date = WorkoutDate.find(params[:id])
    @workout_date.destroy

    respond_to do |format|
      format.html { redirect_to(workout_dates_url) }
      format.xml  { head :ok }
    end
  end
end
