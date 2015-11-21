#ifndef D_2015_11_17_H
#define D_2015_11_17_H

#include<cstdlib>
#include<vector>
#include<iostream>

namespace base
{

	class discriminator
	{
		public:
			discriminator();
			discriminator(std::vector<double> w);
			discriminator(size_t);
			double& W(size_t index);
			size_t W_size();
			bool match(std::vector<double> target);
		private:
			std::vector<double> w;
	};

	template<typename O>
		class d_list
		{
			public:
				d_list();
				d_list(O other);
				virtual ~d_list();
				void add_node(O output,discriminator d);
				void remove_end();
				O run(std::vector<double> input);
				long run_cost(std::vector<double> input);
				void run_history(std::vector<double> input,void(*progress)(discriminator,int,O));
				void run_history(std::vector<double> input,std::vector<discriminator>&,std::vector<O>&);
				size_t size();
				void get_data(std::vector<discriminator>&,std::vector<O>&);
			protected:
				std::vector<discriminator> d;
				std::vector<O> output;
				O other;
				size_t sizel;
		};

	template<typename O>
		class d_creater
		{
			public:
				d_creater(std::vector<O> outputs);
				void add_case(std::vector<double> t,O c);
				d_list<O> study(O ohter,double rho,
						void(*progress)(long,int,int) = 
						[](long ri,int i,int s){return;});
				d_list<O> optimisation(d_list<O> target,
						void(*progress)(long,int,int) = 
						[](long ri,int i,int s){return;});
				size_t case_size();
				size_t class_size();
			private:
				std::vector<std::vector<double> > cases;
				std::vector<unsigned long> classes;
				std::vector<O> outputs;
		};

	//ここからテンプレート関数の定義

	//--d_list--
	template<typename O>
		d_list<O>::d_list()
		{}

	template<typename O>
		d_list<O>::d_list(O other)
		:other(other),sizel(0)
		{}

	template<typename O>
		d_list<O>::~d_list()
		{}

	template<typename O>
		void d_list<O>::add_node(O output,discriminator d)
		{
			this->d.push_back(d);
			this->output.push_back(output);
			sizel++;
		}

	template<typename O>
		void d_list<O>::remove_end()
		{
			d.pop_back();
			output.pop_back();
			sizel--;
		}

	template<typename O>
		size_t d_list<O>::size()
		{ return sizel; }


	template<typename O>
		O d_list<O>::run(std::vector<double> input)
		{
			size_t i = 0;
			while(true)
			{
				if(i >= sizel)
				{
					return other;
				}
				else if(!d[i].match(input))
				{
					i++;
				}
				else if(d[i].match(input))
				{
					return output[i];
				}
			}
		}

	template<typename O>
		void d_list<O>::run_history(std::vector<double> input,void(*progress)(discriminator,int,O))
		{
			size_t i = 0;
			while(true)
			{
				if(i >= sizel)
				{
					return;
				}
				else if(!d[i].match(input))
				{
					progress(d[i],i,output[i]);
					i++;
				}
				else if(d[i].match(input))
				{
					progress(d[i],i,output[i]);
					return;
				}
			}
		}

	template<typename O>
		void d_list<O>::run_history(std::vector<double> input,std::vector<discriminator >& ow,std::vector<O>& oo)
		{
			size_t i = 0;
			while(true)
			{
				if(i >= sizel)
				{
					return;
				}
				else if(!d[i].match(input))
				{
					ow.push_back(d[i]);
					oo.push_back(output[i]);
					i++;
				}
				else if(d[i].match(input))
				{
					ow.push_back(d[i]);
					oo.push_back(output[i]);
					return;
				}
			}
		}

	template<typename O>
		long d_list<O>::run_cost(std::vector<double> input)
		{
			size_t i = 0;
			while(true)
			{
				if(i >= sizel)
				{
					return -1;
				}
				else if(!d[i].match(input))
				{
					i++;
				}
				else if(d[i].match(input))
				{
					return static_cast<long>(i);
				}
			}
		}

	template<typename O>
		void d_list<O>::get_data(std::vector<discriminator>& ds
				,std::vector<O>& os)
		{
			ds = d;
			os = output;
			os.push_back(other);
		}

	//--d_creater--
	
	template<typename O>
		d_creater<O>::d_creater(std::vector<O> outputs)
		:outputs(outputs)
		{}

	template<typename O>
		void d_creater<O>::add_case(std::vector<double> t,O c)
		{
			for(int i = 0;i < outputs.size();i++)
			{
				if(outputs[i] == c)
				{
					classes.push_back(i);
					break;
				}
			}
			cases.push_back(t);
		}

	template<typename O>
		size_t d_creater<O>::case_size()
		{
			return cases.size();
		}
	template<typename O>
		size_t d_creater<O>::class_size()
		{
			return outputs.size();
		}

	namespace
	{
		template<typename T>
			std::vector<T> outof(
					std::vector<T> target,std::vector<int> ref_t)
			{
				std::vector<T> ans;
				int j = 0;
				for(int i = 0;i < target.size();i++)
				{
					while(j < ref_t.size() && i > ref_t[j])
					{
						j++;
					}

					if(i != ref_t[j])
					{
						ans.push_back(target[i]);
					}
				}
				return ans;
			}

		long find_t(std::vector<bool> tar)
		{
			for(long i = 0;i < tar.size();i++)
			{
				if(tar[i])
					return i;
			}
			return -1;
		}

		long find_f(std::vector<bool> tar)
		{
			for(long i = 0;i < tar.size();i++)
			{
				if(!tar[i])
					return i;
			}
			return -1;
		}
	}

	template<typename O>
		d_list<O> d_creater<O>::study(O other,double rho,void(*progress)(long,int,int))
		{
			d_list<O> list(other);
			std::vector< std::vector<double> > ref_case = cases;
			std::vector<unsigned long> ref_class = classes;
			std::vector<discriminator> d(class_size());

			for(int i = 0;i < class_size();i++)
			{
				d[i] = discriminator(cases[0].size());
				d[i].W(0) = 1;
			}
			std::vector<bool> wb(class_size(),false);
			do
			{
				for(int i = 0;i < class_size();i++)
				{
					d[i] = discriminator(cases[0].size());
					d[i].W(0) = 1;
				}
				do
				{
					
					for(int i = 0;i < ref_case.size();i++)
					{
						int c = ref_class[i];
						if(!d[c].match(ref_case[i]))
						{
							for(int j = 0;j < ref_case[i].size();j++)
								d[c].W(j) += rho*ref_case[i][j];
						}
					}

					for(int i = 0;i < ref_case.size();i++)
					{
						int c = ref_class[i];
						for(int k = 0;k < class_size();k++)
						{
							if(d[k].match(ref_case[i]) && c != k)
							{
								for(int j = 0;j < ref_case[i].size();j++)
									d[k].W(j) -= rho*ref_case[i][j];
							}
						}
					}

					wb = std::vector<bool>(class_size(),false);

					for(int i = 0;i < ref_case.size();i++)
					{
						int c = ref_class[i];
						wb[c] = wb[c] || d[c].match(ref_case[i]);
					}

					for(int i = 0;i < ref_case.size();i++)
					{
						int c = ref_class[i];
						for(int j = 0;j < class_size();j++)
						{
							if(c != j)
							{
								wb[j] = wb[j] && !d[j].match(ref_case[i]);
							}
						}
					}
				}
				while(find_t(wb) == -1);

				std::vector<int> outt;

				long rindex = find_t(wb);
				list.add_node(outputs[rindex],d[rindex]);
				for(int i = 0;i < ref_case.size();i++)
				{
					if(d[rindex].match(ref_case[i]) && rindex == ref_class[i])
					{
						outt.push_back(i);
					}
				}
				ref_case = outof(ref_case,outt);
				ref_class = outof(ref_class,outt);
				progress(rindex,ref_case.size(),cases.size());

			}
			while(ref_case.size() > 0);

			return list;
		}
	
	template<typename O>
		d_list<O> d_creater<O>::optimisation(d_list<O> target,void(*progress)(long,int,int))
		{
			std::vector<discriminator> od;
			std::vector<O> oo;
			target.get_data(od,oo);
			d_list<O> ans(oo[oo.size() - 1]);
			oo.pop_back();

			std::vector<std::vector<double> > ref_case = cases;
			std::vector<unsigned long> ref_class = classes;
			std::vector<std::vector<double> > buf_case;
			std::vector<unsigned long> buf_class;

			{
				std::vector<int> outt;
				for(int i = 0;i < ref_case.size();i++)
				{
					if(od[0].match(ref_case[i]))
					{
						outt.push_back(i);
					}
				}
				buf_class = ref_class;
				buf_case = ref_case;
				ref_case = outof(ref_case,outt);
				ref_class = outof(ref_class,outt);
				ans.add_node(oo[0],od[0]);
				progress(0,ref_case.size(),cases.size());
			}

			int sc = 0;
			for(int c = 1;c < od.size();c++)
			{
				if(oo[c - 1] == oo[c])
				{
					bool erflg = true;
					size_t bcs = buf_case.size();
					size_t rcs = ref_case.size();
					for(int i = 0;i < bcs;i++)
					{
						if(outputs[buf_class[i]] == oo[c])
						{
							bool eaflg = false;
							for(int j = 0;j < rcs;j++)
							{
								if(buf_case[i] == ref_case[j])
								{
									eaflg = true;
									break;
								}
							}
							if(!eaflg)
							{
								erflg = erflg && od[c].match(buf_case[i]);
							}
						}
					}
					if(erflg)
					{
						ans.remove_end();
					}
					else
					{
						buf_class = ref_class;
						buf_case = ref_case;
					}
				}
				std::vector<int> outt;
				int ri = -1;
				for(int i = 0;i < ref_case.size();i++)
				{
					if(od[c].match(ref_case[i]))
					{
						outt.push_back(i);
						ri = ref_class[i];
					}
				}
				ans.add_node(oo[c],od[c]);
				ref_case = outof(ref_case,outt);
				ref_class = outof(ref_class,outt);
				progress(ri,ref_case.size(),cases.size());
			}
			return ans;
		}
}

#endif
