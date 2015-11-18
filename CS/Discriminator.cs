using System;
using System.Collections.Generic;
using System.Linq;

namespace TPRS
{

	//識別共通木
	public class D_List<O>
		where O :IEquatable<O>
	{
		public D_List(O other)
		{
			this.other = other;
			begin = null;
			end = null;
		}
		public override string ToString()
		{
			string ans = "";
			Node n = begin;
			while(n != null)
			{
				if(n.next != null)
					ans += n.output + "->" + n.next.output + "\n";
				ans += "(" + n.d.W[0];
				for(int i = 1;i < n.d.W.Length;i++)
					ans += "," + n.d.W[i];
				ans += ")\n";
				n = n.next;
			}
			return ans;
		}
		public void AddNode(O output,Discriminator d)
		{
			Node n = new Node(output,d);
			if(end != null)
				end.next = n;
			end = n;
			if(begin == null)
				begin = n;
		}
		public void RemoveEnd()
		{
			Node n = begin;
			if(n != null && n != end)
			{
				while(n.next != end)
					n = n.next;
				n.next = null;
				end = n;
			}
		}
		public void RemoveBegin()
		{
			if(begin != null)
				begin = begin.next;
		}
		public O Run(double[] input)
		{
			Node n = begin;
			while(true)
			{
				if(n == null)
				{
					return other;
				}
				else if(!n.d.Match(input))
				{
					n = n.next;
				}
				else if(n.d.Match(input))
				{
					return n.output;
				}
			}
		}
		protected class Node
		{
			public Node()
			{
				next = null;
				d = null;
			}
			public Node(O o,Discriminator d)
			{
				next = null;
				this.d = d;
				this.output = o;
			}
			public Node next;
			public Discriminator d;
			public O output;
		}

		protected Node begin;
		protected Node end;
		protected O other;
	}

	//識別器
	public class Discriminator
		:CR_Tree.CRT_IO< double[] >,IEquatable<Discriminator>
	{
		public Discriminator(){}
		public Discriminator(double[] w)
		{this.w = w;}
		public double[] W
		{
			get{return w;}
			set{w = value;}
		}
		public bool Match(double[] target)
		{

			double ans = 0;
			for(int i = 0;i < w.Length;i++)
			{
				ans += w[i]*target[i];
			}

			if(ans >= 0)
				return true;
			else
				return false;

		}
		public bool Equals(Discriminator d)
		{
			if(w.Length != d.w.Length)
				return false;

			for(int i = 0;i < w.Length;i++)
			{
				if(this.w[i] != d.w[i])
					return false;
			}

			return true;
		}
		private double[] w;
	}

}
