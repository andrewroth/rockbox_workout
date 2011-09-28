class SetLogEntriesController < ApplicationController
  # GET /set_log_entries
  # GET /set_log_entries.xml
  def index
    @set_log_entries = SetLogEntry.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @set_log_entries }
    end
  end

  # GET /set_log_entries/1
  # GET /set_log_entries/1.xml
  def show
    @set_log_entry = SetLogEntry.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @set_log_entry }
    end
  end

  # GET /set_log_entries/new
  # GET /set_log_entries/new.xml
  def new
    @set_log_entry = SetLogEntry.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @set_log_entry }
    end
  end

  # GET /set_log_entries/1/edit
  def edit
    @set_log_entry = SetLogEntry.find(params[:id])
  end

  # POST /set_log_entries
  # POST /set_log_entries.xml
  def create
    @set_log_entry = SetLogEntry.new(params[:set_log_entry])

    respond_to do |format|
      if @set_log_entry.save
        format.html { redirect_to(@set_log_entry, :notice => 'Set log entry was successfully created.') }
        format.xml  { render :xml => @set_log_entry, :status => :created, :location => @set_log_entry }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @set_log_entry.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /set_log_entries/1
  # PUT /set_log_entries/1.xml
  def update
    @set_log_entry = SetLogEntry.find(params[:id])

    respond_to do |format|
      if @set_log_entry.update_attributes(params[:set_log_entry])
        format.html { redirect_to(@set_log_entry, :notice => 'Set log entry was successfully updated.') }
        format.xml  { head :ok }
        format.js  { render(:text => "") }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @set_log_entry.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /set_log_entries/1
  # DELETE /set_log_entries/1.xml
  def destroy
    @set_log_entry = SetLogEntry.find(params[:id])
    @set_log_entry.destroy

    respond_to do |format|
      format.html { redirect_to(set_log_entries_url) }
      format.xml  { head :ok }
    end
  end
end
