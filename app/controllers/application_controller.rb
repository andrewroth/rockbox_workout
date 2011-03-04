class ApplicationController < ActionController::Base
  protect_from_forgery
  include OpenFlashChart::Controller
  include OpenFlashChart

  before_filter :authenticate

  # add an initializer with
  #
  # module Auth
  #   USERNAME = 'username'
  #   PASSWORD = 'password'
  # end
  #
  # Add it to #{shared_path}/config/initializers/auth.rb on your server.
  if defined?(Auth)
    def authenticate
      authenticate_or_request_with_http_basic do |username, password|
        username == Auth::USERNAME && password == Auth::PASSWORD
      end
    end
  end
end
