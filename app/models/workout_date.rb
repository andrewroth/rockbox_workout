class WorkoutDate < ActiveRecord::Base
  include Csv

  belongs_to :workout
end
