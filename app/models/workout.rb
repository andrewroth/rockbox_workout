class Workout < ActiveRecord::Base
  include Csv

  has_many :workout_exercises
  has_many :exercises, :through => :workout_exercises

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL").collect(&:workout).flatten.uniq
    end
  end

end
