import Link from 'next/link'
import Layout from '../components/Layout'


export default function Home() {
  return (
    <Layout title="Autoclicker For Linux" home={true}>
    <p className="text-gray-50">This website is currently under development</p>
  </Layout>
  )
}
