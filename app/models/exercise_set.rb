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

  def value_of(variable, n)
    functions.find_all_by_variable(variable).each do |f|
      if ((f.min_n.present? && n >= f.min_n) || !f.min_n.present?) &&
        ((f.max_n.present? && n <= f.max_n) || !f.max_n.present?)
        value = f.base.to_i + (n * f.inc.to_i)
        if f.round_to_nearest.present?
          r = f.round_to_nearest
          value = ((value + r / 2.0).to_i / r) * r
        end
        return value
      end
    end
    return nil
  end
end
