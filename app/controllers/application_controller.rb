class ApplicationController < ActionController::Base
  protect_from_forgery
  include OpenFlashChart::Controller
  include OpenFlashChart

  before_filter :authenticate

  def authenticate
    authenticate_or_request_with_http_basic do |username, password|
      username == "foo" && password == "bar"
    end
  end
end
