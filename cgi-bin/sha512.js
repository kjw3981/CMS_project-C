/**
 * SHA-512
 * @copyright 2015 yujakudo
 * 菴ｿ縺�婿��
 * var s_hash = yjd.sha512.get(�懈枚蟄怜���);
 */

var yjd = yjd || {};
yjd.sha512 = yjd.sha512 || {};

(function(sha512){

	//	螳壽焚K
    sha512.au32_K = new Uint32Array(160);
    sha512.au32_K.set([
			0x428a2f98, 0xd728ae22, 0x71374491, 0x23ef65cd, 0xb5c0fbcf, 0xec4d3b2f, 0xe9b5dba5, 0x8189dbbc,
			0x3956c25b, 0xf348b538, 0x59f111f1, 0xb605d019, 0x923f82a4, 0xaf194f9b, 0xab1c5ed5, 0xda6d8118,
			0xd807aa98, 0xa3030242, 0x12835b01, 0x45706fbe, 0x243185be, 0x4ee4b28c, 0x550c7dc3, 0xd5ffb4e2,
			0x72be5d74, 0xf27b896f, 0x80deb1fe, 0x3b1696b1, 0x9bdc06a7, 0x25c71235, 0xc19bf174, 0xcf692694,
			0xe49b69c1, 0x9ef14ad2, 0xefbe4786, 0x384f25e3, 0x0fc19dc6, 0x8b8cd5b5, 0x240ca1cc, 0x77ac9c65,
			0x2de92c6f, 0x592b0275, 0x4a7484aa, 0x6ea6e483, 0x5cb0a9dc, 0xbd41fbd4, 0x76f988da, 0x831153b5,
			0x983e5152, 0xee66dfab, 0xa831c66d, 0x2db43210, 0xb00327c8, 0x98fb213f, 0xbf597fc7, 0xbeef0ee4,
			0xc6e00bf3, 0x3da88fc2, 0xd5a79147, 0x930aa725, 0x06ca6351, 0xe003826f, 0x14292967, 0x0a0e6e70,
			0x27b70a85, 0x46d22ffc, 0x2e1b2138, 0x5c26c926, 0x4d2c6dfc, 0x5ac42aed, 0x53380d13, 0x9d95b3df,
			0x650a7354, 0x8baf63de, 0x766a0abb, 0x3c77b2a8, 0x81c2c92e, 0x47edaee6, 0x92722c85, 0x1482353b,
			0xa2bfe8a1, 0x4cf10364, 0xa81a664b, 0xbc423001, 0xc24b8b70, 0xd0f89791, 0xc76c51a3, 0x0654be30,
			0xd192e819, 0xd6ef5218, 0xd6990624, 0x5565a910, 0xf40e3585, 0x5771202a, 0x106aa070, 0x32bbd1b8,
			0x19a4c116, 0xb8d2d0c8, 0x1e376c08, 0x5141ab53, 0x2748774c, 0xdf8eeb99, 0x34b0bcb5, 0xe19b48a8,
			0x391c0cb3, 0xc5c95a63, 0x4ed8aa4a, 0xe3418acb, 0x5b9cca4f, 0x7763e373, 0x682e6ff3, 0xd6b2b8a3,
			0x748f82ee, 0x5defb2fc, 0x78a5636f, 0x43172f60, 0x84c87814, 0xa1f0ab72, 0x8cc70208, 0x1a6439ec,
			0x90befffa, 0x23631e28, 0xa4506ceb, 0xde82bde9, 0xbef9a3f7, 0xb2c67915, 0xc67178f2, 0xe372532b,
			0xca273ece, 0xea26619c, 0xd186b8c7, 0x21c0c207, 0xeada7dd6, 0xcde0eb1e, 0xf57d4f7f, 0xee6ed178,
			0x06f067aa, 0x72176fba, 0x0a637dc5, 0xa2c898a6, 0x113f9804, 0xbef90dae, 0x1b710b35, 0x131c471b,
			0x28db77f5, 0x23047d84, 0x32caab7b, 0x40c72493, 0x3c9ebe0a, 0x15c9bebc, 0x431d67c4, 0x9c100d4c,
			0x4cc5d4be, 0xcb3e42b6, 0x597f299c, 0xfc657e2a, 0x5fcb6fab, 0x3ad6faec, 0x6c44198c, 0x4a475817
	]);

	/**
	 * 繧ｯ繝ｩ繧ｹCalc
	 * SHA-512繧定ｨ育ｮ励☆繧九け繝ｩ繧ｹ
	 */
	sha512.Calc = function() {
		this.au32_H = new Uint32Array(16);	//	繝ｬ繧ｸ繧ｹ繧ｿ
		this.au32_buffer = null;			//	繝舌ャ繝輔ぃ
		this.n_blocks = 0;					//	繝舌ャ繝輔ぃ蜀��繝悶Ο繝�け謨ｰ. 

		/**
		 * 蛻晄悄蛹�.
		 */
		this.Init = function() {
            this.au32_H.set([
                0x6a09e667, 0xf3bcc908, 0xbb67ae85, 0x84caa73b,
                0x3c6ef372, 0xfe94f82b, 0xa54ff53a, 0x5f1d36f1,
                0x510e527f, 0xade682d1, 0x9b05688c, 0x2b3e6c1f,
                0x1f83d9ab, 0xfb41bd6b, 0x5be0cd19, 0x137e2179
            ]);
        };

		/**
		 * 螟画鋤蜃ｦ逅�.
		 */
		this.Transform = function() {
			var a0, a1, b0, b1, c0, c1, d0, d1, e0, e1, f0, f1, g0, g1, h0, h1, e1_o, a1_o;
			var t, w0, w1, w20, w21, wf0, wf1, w1_o;
			var T10, T11, T11_o, T20, T21, T21_o;
			var au32_W = new Uint32Array(160);
			var n_idx = 0;
			var K = sha512.au32_K;
			var digit32 = 0x100000000;	//	4294967296;

			for( var i=0; i<this.n_blocks; i++) {

				//	1. Prepare the message schedule W:
				for(t=0; t<32; t++) {
					au32_W[t] = this.au32_buffer[n_idx++];
				}
				for(t=32; t<160; t+=2) {
					//	W[t-2]
					w21 = au32_W[t-3];
					w20 = au32_W[t-4];
					//	W[t-15]
					wf1 = au32_W[t-29];
					wf0 = au32_W[t-30];
					//	Wt = SSIG1(W(t-2)) + W(t-7) + SSIG0(W(t-15)) + W(t-16)
					w1_o = ((((w21>>>19) | (w20<<13)) ^ ((w21<<3) | (w20>>>29)) ^ ((w21>>>6) | (w20<<26))) >>>0)
                        + au32_W[t-13]
                        + ((((wf1>>>1) | (wf0<<31)) ^ ((wf1>>>8) | (wf0<<24)) ^ ((wf1>>>7) | (wf0<<25))) >>>0)
                        + au32_W[t-31];
					w1 = w1_o >>> 0;
					au32_W[t+1] = w1;
					au32_W[t] = (((w20>>>19) | (w21<<13)) ^ ((w20<<3) | (w21>>>29)) ^ (w20>>>6))
                        + au32_W[t-14]
                        + (((wf0>>>1) | (wf1<<31)) ^ ((wf0>>>8) | (wf1<<24)) ^ (wf0>>>7))
                        + au32_W[t-32] + ((w1_o - w1) / digit32);
				}

				//	2. Initialize the working variables:
				a1 = this.au32_H[1];
				a0 = this.au32_H[0];
				b1 = this.au32_H[3];
				b0 = this.au32_H[2];
				c1 = this.au32_H[5];
				c0 = this.au32_H[4];
				d1 = this.au32_H[7];
				d0 = this.au32_H[6];
				e1 = this.au32_H[9];
				e0 = this.au32_H[8];
				f1 = this.au32_H[11];
				f0 = this.au32_H[10];
				g1 = this.au32_H[13];
				g0 = this.au32_H[12];
				h1 = this.au32_H[15];
				h0 = this.au32_H[14];

				//	3. Perform the main hash computation:
				for(t=0; t<160; t+=2) {
					//	T1 = h + BSIG1(e) + CH(e,f,g) + Kt + Wt
					T11_o = h1
                        + ((((e1>>>14) | (e0<<18)) ^ ((e1>>>18) | (e0<<14)) ^ ((e1<<23) | (e0>>>9))) >>>0)
                        + (((e1 & f1) ^ (~e1 & g1)) >>>0)
                        + K[t+1]
                        + au32_W[t+1];
					T11 = T11_o >>> 0;
					T10 = h0
                        + (((e0>>>14) | (e1<<18)) ^ ((e0>>>18) | (e1<<14)) ^ ((e0<<23) | (e1>>>9)))
                        + ((e0 & f0) ^ (~e0 & g0))
                        + K[t] + au32_W[t] + ((T11_o - T11) / digit32);
					//	T2 = BSIG0(a) + MAJ(a,b,c);
					T21_o = (((((a1>>>28) | (a0<<4)) ^ ((a1<<30) | (a0>>>2)) ^ ((a1<<25) | (a0>>>7)))) >>>0)
                        + (((a1 & b1) ^ (a1 & c1) ^ (b1 & c1)) >>>0);
					T21 = T21_o >>> 0;
					T20 = (((a0>>>28) | (a1<<4)) ^ ((a0<<30) | (a1>>>2)) ^ ((a0<<25) | (a1>>>7)))
                        + ((a0 & b0) ^ (a0 & c0) ^ (b0 & c0)) + ((T21_o - T21) / digit32);
                    
					h1 = g1;	h0 = g0;
					g1 = f1;	g0 = f0;
					f1 = e1;	f0 = e0;
					e1_o = d1 + T11;	e1 = e1_o >>>0;	e0 = d0 + T10 + ((e1_o > e1)? 1: 0);
					d1 = c1;	d0 = c0;
					c1 = b1;	c0 = b0;
					b1 = a1;	b0 = a0;
					a1_o = T11 + T21;	a1 = a1_o >>>0;	a0 = T10 + T20 + ((a1_o > a1)? 1: 0);
				}
                
				//	4. Compute the intermediate hash value H(i)
				this.au32_H[1]  += a1;
				this.au32_H[0]  += a0 + (this.au32_H[1]<a1? 1: 0);
				this.au32_H[3]  += b1;
				this.au32_H[2]  += b0 + (this.au32_H[3]<b1? 1: 0);
				this.au32_H[5]  += c1;
				this.au32_H[4]  += c0 + (this.au32_H[5]<c1? 1: 0);
				this.au32_H[7]  += d1;
				this.au32_H[6]  += d0 + (this.au32_H[7]<d1? 1: 0);
				this.au32_H[9]  += e1;
				this.au32_H[8]  += e0 + (this.au32_H[9]<e1? 1: 0);
				this.au32_H[11] += f1;
				this.au32_H[10] += f0 + (this.au32_H[11]<f1? 1: 0);
				this.au32_H[13] += g1;
				this.au32_H[12] += g0 + (this.au32_H[13]<g1? 1: 0);
				this.au32_H[15] += h1;
				this.au32_H[14] += h0 + (this.au32_H[15]<h1? 1: 0);
			}
		};

		/**
		 * 繝代ョ繧｣繝ｳ繧ｰ
		 * @param n_length 繝��繧ｿ髟ｷ(byte).
		 */
		this.Padding = function(n_length) {
			var n_mod = n_length % 4;
			var n_idx = (n_length - n_mod) / 4;
			this.au32_buffer[n_idx++] |= (0x80000000 >>> (n_mod * 8));
			while(n_idx % 32!=28) {
				this.au32_buffer[n_idx++] = 0;
			}
			var n_bit_len = n_length * 8;
			this.au32_buffer[n_idx++] = 0;
			this.au32_buffer[n_idx++] = 0;
			this.au32_buffer[n_idx++] = Math.floor(n_bit_len / 0x100000000);
			this.au32_buffer[n_idx++] = n_bit_len;
			this.n_blocks = n_idx / 32;
		};

		/**
		 * 譁�ｭ怜�繧旦TF-8縺ｫ螟画鋤縺励※繝舌ャ繝輔ぃ縺ｫ繧ｻ繝�ヨ縺吶ｋ.
		 * @param s_str	譁�ｭ怜�.
		 * @return 繧ｵ繧､繧ｺ.(byte)
		 */
		this.SetString = function(s_str) {
			var n_max_bytes = s_str.length * 4;
			var n_buff_size = (Math.floor((n_max_bytes + 16) / 128) + 1) * 32;
			this.au32_buffer = new Uint32Array(n_buff_size);
			var n_len = s_str.length;
			var n_idx = 0, n_shift = 24, n_code;
			for(var i=0; i<n_len; i++) {
				n_code = s_str.charCodeAt(i);
				if(n_code < 0x80) {
					this.au32_buffer[n_idx] |= (n_code << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
				} else if(n_code < 0x800) {
					this.au32_buffer[n_idx] |= ((0xc0 | (n_code >>> 6)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
					this.au32_buffer[n_idx] |= ((0x80 | (n_code & 0x3f)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
				} else if(n_code < 0xd800 || n_code >= 0xe000) {
					this.au32_buffer[n_idx] |= ((0xe0 | (n_code >>> 12)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
					this.au32_buffer[n_idx] |= ((0x80 | ((n_code>>>6) & 0x3f)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
					this.au32_buffer[n_idx] |= ((0x80 | (n_code & 0x3f)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
				} else {
					this.au32_buffer[n_idx] |= ((0xf0 | (n_code >>>18)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
					this.au32_buffer[n_idx] |= ((0x80 | ((n_code>>>12) & 0x3f)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
					this.au32_buffer[n_idx] |= ((0x80 | ((n_code>>>6) & 0x3f)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
					this.au32_buffer[n_idx] |= ((0x80 | (n_code & 0x3f)) << n_shift);	(n_shift==0)? (n_idx++, n_shift=24): n_shift -= 8;
				}
			}
			return n_idx * 4 + (24 - n_shift) / 8;
		};


		/**
		 * 繝上ャ繧ｷ繝･蛟､繧呈枚蟄怜�縺ｧ蠕励ｋ.
		 * @return 譁�ｭ怜�(128譁�ｭ�).
		 */
		this.GetValueByStr = function() {
			var s_str = '';
			for(var i=0; i<16; i++) {
                s_str += ('00000000' + this.au32_H[i].toString(16)).substr(-8);
			}
			return s_str;
		};

		/**
		 * 繝上ャ繧ｷ繝･蛟､繧呈枚蟄怜�縺ｧ蜿門ｾ�.
		 */
		this.GetString = function(s_str) {
			this.Init();
			var n_bytes = this.SetString(s_str);
			this.Padding(n_bytes);
			this.Transform();
			return this.GetValueByStr();
		};
	};

	/**
	 *	繝上ャ繧ｷ繝･蛟､繧呈枚蟄怜�縺ｧ蠕励ｋ.
	 *	@param m_arg 迴ｾ蝨ｨ縺ｯ譁�ｭ怜�縺ｮ縺ｿ蟇ｾ蠢�.
	 *	@return 繝上ャ繧ｷ繝･蛟､縺ｮ16騾ｲ譁�ｭ怜�.��128譁�ｭ暦ｼ�
	 */
	sha512.getString = function(m_arg) {
		var calc = new sha512.Calc();
		return calc.GetString(m_arg);
	};

	/**
	 * 繝繧､繧ｸ繧ｧ繧ｹ繝医ｒ蠕励ｋ.
	 * @param s_user	繝ｦ繝ｼ繧ｶ繝ｼID
	 * @param s_realm	realm
	 * @param s_passwd	繝代せ繝ｯ繝ｼ繝�
	 * @param s_method	GET or POST
	 * @param s_uri		URI
	 * @param s_nonce	nonce
	 * @param s_cnonce	cnonce
	 * @param s_nc		nc.
	 * @param s_qgp		qgp.
	 * @return 繝上ャ繧ｷ繝･蛟､縺ｮ譁�ｭ怜�.
	 */
	sha512.getDigest = function (s_user, s_realm, s_passwd, s_method, s_uri, s_nonce, s_cnonce, s_nc, s_qgp) {
		var calc = new yjd.Calc();
		var s_a1 = calc.GetOfString(s_user + ':' + s_realm + ':'+ s_passwd);
		var s_a2 = calc.GetOfString(s_method + ':' + s_uri);
		return calc.GetOfString(s_a1 + ':' + s_nonce + ':' + s_nc + ':' + s_cnonce + ':' + s_qgp + ':' + s_a2);
	}

	/**
	 * nonce縺ｮ蜿門ｾ�.
	 * @param i_length 譁�ｭ怜�髟ｷ. 逵∫払蛹�.
	 * @return nonce縺ｮ譁�ｭ怜�
	 */
	sha512.createNonce = function (i_length) {
		if(i_length===undefined) {
			i_length = 52;
		}
		var s_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		var i_chars = s_chars.length;
		var s_nonce = '';
		for(var i=0; i<i_length; i++) {
			s_nonce += s_chars.charAt(Math.floor(Math.random() * i_chars));
		}
		return s_nonce;
	};

}(yjd.sha512));