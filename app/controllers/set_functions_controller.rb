class SetFunctionsController < ApplicationController
  # GET /set_functions
  # GET /set_functions.xml
  def index
    @set_functions = SetFunction.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @set_functions }
    end
  end

  # GET /set_functions/1
  # GET /set_functions/1.xml
  def show
    @set_function = SetFunction.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @set_function }
    end
  end

  # GET /set_functions/new
  # GET /set_functions/new.xml
  def new
    @set_function = SetFunction.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @set_function }
    end
  end

  # GET /set_functions/1/edit
  def edit
    @set_function = SetFunction.find(params[:id])
  end

  # POST /set_functions
  # POST /set_functions.xml
  def create
    @set_function = SetFunction.new(params[:set_function])

    respond_to do |format|
      if @set_function.save
        format.html { redirect_to(@set_function, :notice => 'Set function was successfully created.') }
        format.xml  { render :xml => @set_function, :status => :created, :location => @set_function }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @set_function.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /set_functions/1
  # PUT /set_functions/1.xml
  def update
    @set_function = SetFunction.find(params[:id])

    respond_to do |format|
      if @set_function.update_attributes(params[:set_function])
        format.html { redirect_to(@set_function, :notice => 'Set function was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @set_function.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /set_functions/1
  # DELETE /set_functions/1.xml
  def destroy
    @set_function = SetFunction.find(params[:id])
    @set_function.destroy

    respond_to do |format|
      format.html { redirect_to(set_functions_url) }
      format.xml  { head :ok }
    end
  end
end
