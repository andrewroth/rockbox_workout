class ExerciseSet < ActiveRecord::Base
  include Csv

  belongs_to :exercise
  has_many :functions
  has_many :set_log_entries, :foreign_key => "workout_set_id"

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL").collect(&:workout).flatten.collect(&:exercises).flatten.collect(&:exercise_sets).flatten.uniq
    end 
  end 

  def name_with_exercise
    "#{exercise.name} - #{name} [#{exercise.exercise_type.variables.join(', ')}]"
  end

  def value_of(variable, n, workout = nil)
    functions.find_all_by_variable(variable).each do |f|
      if workout.present? && f.workout && f.workout != workout
        next
      end
      value = f.value(n)
      return value if value
    end
    return nil
  end

  def weights_goal_hash
    Hash[set_log_entries.collect(&:exercise_log_entry).collect{ |sle| 
      n = sle.n.to_i
      [ n, self.value_of("weight", n) ]
    }]
  end

  def weights_goal_arr
    weights_goal_hash.values
  end

  def reps_goal_hash
    n_v = set_log_entries.collect(&:exercise_log_entry).collect{ |sle| 
      n = sle.n.to_i
      [ n, self.value_of("reps", n).to_i ]
    }
    n_v.sort!{ |n1,n2| n1.first <=> n2.first }
    r = ActiveSupport::OrderedHash.new
    n_v.each do |n,v|
      r[n] = v
    end
    return r
  end

  def reps_goal_arr
    reps_goal_hash.values
  end
end
