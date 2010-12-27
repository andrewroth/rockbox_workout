class ExerciseSet < ActiveRecord::Base
  include Csv

  belongs_to :exercise
  has_many :functions

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL").collect(&:workout).flatten.collect(&:exercises).flatten.collect(&:exercise_sets).flatten.uniq
    end 
  end 

  def name_with_exercise
    "#{exercise.name} - #{name} [#{exercise.exercise_type.variables.join(', ')}]"
  end
end
