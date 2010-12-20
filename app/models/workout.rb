class Workout < ActiveRecord::Base
  include Csv

  has_many :workout_exercises
end
